//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "node.h"
#include<bitset>
#include<vector>
#include <fstream>
#include <string>
#include "custom_message_m.h"
using namespace std;

Define_Module(Node);

string Node::frame(string payload) {
    string s;
    s += '$';
    for (int i = 0; i < payload.length(); i++) {
        if (payload[i] == '$' || payload[i] == '/')
            s += '/';
        s += payload[i];
    }
    s += '$';
    return s;
}

string Node::deframe(string frame) {
    string payload;
    for (int i = 1; i < frame.length() - 1; i++) {
        if (frame[i] == '/' && (frame[i + 1] == '/' || frame[i + 1] == '$')) {
            payload += frame[i + 1];
            i++;
        } else
            payload += frame[i];
    }
    return payload;
}

// INITIALIZE LOGGERS WITH EMPTY STRINGS
int Node::processError(string errorCode, string &framedPayload, string &logger,
        string &logger2, int &delay) {
    int numberSent = 1; // number of packets sent [0 for loss, 2 for duplicates]
    int errorDelay = errorDelayTime;

    // Modified
    if (errorCode[0] == '1') {
        // Modification Logic: Modify 9th bit (1st bit in second byte)
        bitset<8> char1 = bitset<8>(framedPayload[1]);
        char1[0] = not char1[0];
        framedPayload[1] = char1.to_string();

        // Log Modified Bit
        logger += "Modified [9] ,";

    } else {
        // Log No Modification
        logger += "Modified [-1] ,";
    }

    //loss
    if (errorCode[1] == '1') {
        // indicate no sent packet (LOSS)
        numberSent = 0;

        // Log Loss
        logger += "Lost [Yes] ,";
    } else {
        // Log No Loss
        logger += "Lost [No] ,";
    }

    // duplicate
    if (errorCode[2] == '1') {
        // Log Duplicate (Both Frames)
        logger2 += logger;
        logger += "Duplicate [1] ,";
        logger2 += "Duplicate [2] ,";

        // carry out duplicate logic (+0.1 (DUP) delay)

        // indicate 2 sent packets if NO LOSS (DUPLICATE)
        if (errorCode[1] == '0')
            numberSent = 2;
    } else {
        // Log No Duplicate
        logger += "Duplicate [0] ,";
    }

    //delay
    if (errorCode[3] == '1') {
        logger += "Delay [" + to_string(errorDelay) + "].";
        // Log error to duplicate packet
        if (errorCode[2] == '1') {
            logger2 += "Delay [" + to_string(errorDelay) + "].";
        }
        delay += errorDelay;
        // carry out delay logic (+ delay error (ED) stored fel init)
    } else {
        logger += "Delay [0] ,";
        if (errorCode[2] == '1') {
            logger2 += "Delay [0].";
        }
    }
}

int Node::calcParityBit(int header, string payload) {
    bitset<8> m(header);
    for (int i = 0; i < payload.size(); i++) {
        m = (m ^ bitset<8>(payload[i]));
    }
    return (int) (m.to_ulong());
}

void Node::GoBackN(int maxSeqBunch, int seqN) {
    expectedSeqNumber = seqN;
    int arraySize = values.size();
    currentWindowSize = min(windowSize, arraySize - maxSeqBunch);

    for (int i = seqN; i < currentWindowSize; i++)
    {
        Custom_message_Base *timerMessage = new Custom_message_Base("timeOut");
        timerMessage->setM_Header(i);
        timerMessage->setM_Trailer(currentWindowSize + maxSeqBunch);

        scheduleAt(simTime() + timeout, timerMessage);

        handleSend(values[i + maxSeqBunch].second, i, values[i + maxSeqBunch].second);
    }

}

void Node::readFile() {
    string inputFileName =
            "D:/uni_courses/sem7/networks-1/go-back-N-DLL/project_networks/src/input0.txt";
    std::ifstream file(inputFileName);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << inputFileName << endl;
        return;
    }

    cout << "Reading data from file: " << inputFileName << endl;

    std::string line;
    string id, description;
    while (std::getline(file, line)) {
        id = line.substr(0, 4);
        description = line.substr(5);
        values.push_back( { id, description });
        cout << "id = " << id << " , description" << description << endl;
    }

    file.close();
}

void Node::initialize() {
    readFile();

    //  set node id
    const char *nodeName = getName();
    nodeID = -1;
    if (strcmp(nodeName, "node2") == 0)
        nodeID = 1;
    else
        nodeID = 0;

    windowSize = par("WS").intValue();
    timeout = par("TO").doubleValue();
    packetProcessingTime = par("PT").doubleValue();
    transmissionDelay = par("TD").doubleValue();
    errorDelayTime = par("ED").doubleValue();
    duplicationDelay = par("DD").doubleValue();
    probabilityOfAckLoss = par("LP").intValue();
}

void Node::handleMessage(cMessage *msg) {
    // Get the gate through which the message was received
    cGate *arrivalGate = msg->getArrivalGate();

    // You can access the gate's index to check which gate it arrived at
    int gateIndex = arrivalGate->getIndex();

    // received a message from the coordinator --> sender
    if (gateIndex == 1 && isSender == false) {
        isSender = true;
    }

    // sender handling
    if (isSender) {
        if (strcmp(msg->getName(), "timeOut") == 0)
        {
            if (msg->getM_Trailer() > maxSeqBunch and msg->getM_Header()>=expectedSeqNumber)
            {
                int seqNumber = getM_Header();
                loggedMessage = format(" Time out event at time [{}], at Node[{}] for frame with seq_num=[{}], ",
                        SimTime(), nodeID, seqNumber);
                EV << loggedMessage <<endl;

                GoBackN(maxSeqBunch, expectedSeqNumber);
            }

            return;
        }

        int size = values.size();
        int currentAck = msg->getM_Ack_Num();
        int frameType = getM_Type();

        if (currentAck >= expectedSeqNumber and frameType == 1) {
            expectedSeqNumber = currentAck + 1;
            if (expectedSeqNumber == currentWindowSize) {
                maxSeqBunch += currentWindowSize;
                if (maxSeqBunch == size)
                    return;

                GoBackN(maxSeqBunch, 0);
            }
        } else {
            GoBackN(maxSeqBunch, expectedSeqNumber);
        }
    }
    //receiver handling
    else {
        handleReceive(msg);
    }
}

void Node::handleSend(string payload, int seq_number, string errorCode) {
    double currentTime = SimTime(); //current time

    // Initialize logged message for log 1
    string loggedMessage = format(
            "At time [{}], Node[{}], Introducing channel error with code {}, ",
            currentTime, nodeID, errorCode);
    EV << loggedMessage <<endl;

    // TODO: LOG MESSAGE 1 TO OUTPUT FILE

    // Intialize Delay with processing time delay
    double delay = packetProcessingTime;
    currentTime += delay;

    // create a new custom message
    Custom_message_Base *msg = new Custom_message_Base();

    // set the header with the sequence number
    msg->setM_Header(seq_number);

    // Apply framing to the Payload and set it
    string framedPayload = frame(payload);
    msg->setM_Payload(framedPayload.c_str());

    // calculate the parity bit and add parity to the message trailer
    int parity = calcParityBit(seq_number, framedPayload);
    msg->setM_Trailer(parity);

    // introduce error to Payload
    string logger = "";
    string logger2 = "";
    int numberSent = processError(errorCode, framedPayload, logger, logger2,
            delay);

    // Log Message 2

    loggedMessage =
            format(
                    "At time [{}], Node[{}] sent frame with seq_num=[{}] and payload=[{}] and trailer=[{}],",
                    currentTime, nodeID, seq_number, framedPayload,
                    (bitset<8>(parity)).to_string());

    double dup_delay = duplicationDelay;

    // In case of duplicates
    string loggedMessage2 =
            format(
                    "At time [{}], Node[{}] sent frame with seq_num=[{}] and payload=[{}] and trailer=[{}],",
                    (currentTime + dup_delay), nodeID, seq_number,
                    framedPayload, (bitset<8>(parity)).to_string());

    EV << loggedMessage+logger <<endl;
    // TODO: log message 2 to output file

    delay += transmissionDelay;

    // set the message type to data (because this is the initialization)
    msg->setM_Type(2);

    // set the Ack number
    msg->setM_Ack_Num(15); // needs to be properly set

    if (numberSent == 0)
        return;

    if (numberSent == 1) {
        sendDelayed(msg, delay, "out");
    }

    if (numberSent == 2) {
        // TODO: log message2 2 to output file
        EV << loggedMessage2+logger2 <<endl;
        sendDelayed(msg, delay, "out");
        sendDelayed(msg, (delay + dup_delay), "out");
    }

}

void handleReceive(Custom_message_Base *msg) {
    Custom_message_Base *mmsg = check_and_cast<Custom_message_Base*>(msg);

    string frame = mmsg->getM_Payload();
    string payload = deframe(frame);

    int parity = mmsg->getM_Trailer();
    int par = calcParityBit(mmsg->getM_Header(), frame);

    string AckNack = "";

    if (parity != par)
    {
        mmsg->setM_Type(0);  // NACK
        AckNack = "NACK";
    }
    else
    {
        mmsg->setM_Type(1);  // ACK
        AckNack = "ACK";
    }

    mmsg->setM_Ack_Num(mmsg->getM_Header());

    string AckLoss = "";

    int rand = uniform(0,1)*100;
    if (rand<= probabilityOfAckLoss)
    {
        AckLoss = "Yes";
    }
    else
    {
        send(mmsg, "out");
        AckLoss = "No";
    }


    string loggedMessage =
        format(
           "At time [{}], Sending [{}] with number [{}] , loss [Yes/No ].",
           SimTime(), AckNack, AckLoss);

    EV << loggedMessage << endl;
}
