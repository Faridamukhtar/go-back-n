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

#include"node.h"
#include<bitset>
#include<vector>
#include<fstream>
#include<string>
#include<iostream>

using namespace std;

Define_Module(Node);

string Node::frame(string payload) {
    string s;
    s += '$';
    int psize = payload.size();
    for (int i = 0; i < psize; i++) {
        if (payload[i] == '$' || payload[i] == '/')
            s += '/';
        s += payload[i];
    }
    s += '$';
    return s;
}

string Node::deframe(string frame) {
    string payload;
    int fsize = frame.size();
    for (int i = 1; i <  fsize - 1; i++) {
        if (frame[i] == '/' && (frame[i + 1] == '/' || frame[i + 1] == '$')) {
            payload += frame[i + 1];
            i++;
        } else
            payload += frame[i];
    }
    return payload;
}

// INITIALIZE LOGGERS WITH EMPTY STRINGS
int Node::processError(string errorCode, string &framedPayload, string &logger,string &logger2, double &delay)
{
    int numberSent = 1; // number of packets sent [0 for loss, 2 for duplicates]
    double errorDelay = errorDelayTime;

    // Modified
    if (errorCode[0] == '1') {
        // Modification Logic: Modify 9th bit (1st bit in second byte)
        bitset<8> char1 = bitset<8>(framedPayload[1]);
        char1[0] = not char1[0];
        framedPayload[1] = static_cast<char>(char1.to_ulong());

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
    return numberSent;
}

int Node::calcParityBit(int header, string payload) {
    bitset<8> m(header);
    int psize = payload.size();
    for (int i = 0; i < psize; i++) {
        m = (m ^ bitset<8>(payload[i]));
    }
    return (int) (m.to_ulong());
}

void Node::GoBackN(int maxSeqBunch, int seqN) {
    expectedSeqNumber = seqN;
    int arraySize = values.size();
    currentWindowSize = min(windowSize, arraySize - maxSeqBunch);
    double delay = 0;

    for (int i = seqN; i < currentWindowSize; i++)
    {
        Custom_message_Base *timerMessage = new Custom_message_Base("timeOut");
        timerMessage->setM_Header(i);

        timeoutQueue.push(timerMessage);

        scheduleAt(SimTime(simTime().dbl() + timeout + delay), timerMessage);

        handleSend(values[i + maxSeqBunch].second, i, values[i + maxSeqBunch].first, delay);
        values[i + maxSeqBunch] = {"0000", values[i + maxSeqBunch].second};
        delay+= packetProcessingTime;
    }

}

void Node::readFile(int nodeId) {
    string inputFileName;
    if (nodeId==0)
        inputFileName ="../src/input0.txt";
    else
        inputFileName ="../src/input1.txt";


    ifstream file(inputFileName);
    if (!file.is_open()) {
        return;
    }

    string line;
    string id, description;
    while (getline(file, line)) {
        id = line.substr(0, 4);
        description = line.substr(5);
        values.push_back( { id, description });
    }

    file.close();
}

void Node::initialize() {
    //  set node id
    const char *nodeName = getName();
    nodeID = -1;
    if (strcmp(nodeName, "node2") == 0)
        nodeID = 1;
    else
        nodeID = 0;

    readFile(nodeID);

    windowSize = par("WS").intValue();
    timeout = par("TO").doubleValue();
    packetProcessingTime = par("PT").doubleValue();
    transmissionDelay = par("TD").doubleValue();
    errorDelayTime = par("ED").doubleValue();
    duplicationDelay = par("DD").doubleValue();
    probabilityOfAckLoss = par("LP").intValue();
    lastReceived = -1;
    maxSeqBunch = 0;
    isSender = false;
    pointer = 0;
}

void Node::cancelAllTimeouts() {
    while (!timeoutQueue.empty()) {
        Custom_message_Base* msg = timeoutQueue.front(); // Get the front message
        timeoutQueue.pop(); // Remove it from the queue

        if (msg->isScheduled()) {
            cancelEvent(msg); // Cancel the scheduled event
        }
    }
}

void Node::cancelTimeoutsUpToSeqNumber(int seqNumber) {
    while (!timeoutQueue.empty()) {
        Custom_message_Base* msg = timeoutQueue.front(); // Get the front message
        timeoutQueue.pop(); // Remove it from the queue

        if (msg && msg->getM_Header() <= seqNumber) {
            // Cancel the event if scheduled and sequence number matches
            if (msg->isScheduled()) {
                cancelEvent(msg);
            }
        } else {
            break; // all remaining seq numbers will be greater than the threshold sequence number
        }
    }
}

void Node::handleMessage(cMessage *msg) {
    // Get the gate through which the message was received
    cGate *arrivalGate = msg->getArrivalGate();

    // You can access the gate's index to check which gate it arrived at
    int gateIndex = -1;
    if (arrivalGate)
        gateIndex = arrivalGate->getIndex();

    if (gateIndex == 1)
    {
        isSender = true;
        GoBackN(maxSeqBunch, 0);
        return;
    }

    Custom_message_Base *mmsg = check_and_cast<Custom_message_Base*>(msg);

    // sender handling
    if (isSender)
    {
        if (strcmp(mmsg->getName(), "timeOut") == 0)
        {
            if (mmsg->getM_Header()<expectedSeqNumber)
                return;

            cancelAllTimeouts();

            int seqNumber = mmsg->getM_Header();
            string loggedMessage = "Time out event at time " + to_string(simTime().dbl()) +
                                   ", at Node" + to_string(nodeID) +
                                   " for frame with seq_num=" + to_string(seqNumber) + ", ";
            EV << loggedMessage <<endl;
            writeFile(loggedMessage);

            GoBackN(maxSeqBunch, expectedSeqNumber);

            return;
        }
        int size = values.size();
        int currentAck = mmsg->getM_Ack_Num();
        int frameType = mmsg->getM_Type();
        if (currentAck >= expectedSeqNumber and frameType == 1)
        {
            expectedSeqNumber = currentAck + 1;
            cancelTimeoutsUpToSeqNumber(currentAck);
            if (expectedSeqNumber == currentWindowSize) {
                maxSeqBunch += currentWindowSize;
                if (maxSeqBunch == size)
                    return;
                GoBackN(maxSeqBunch, 0);
            }
        }
    }
    //receiver handling
    else {
        handleReceive(msg);
    }

}

void Node::handleSend(string payload, int seq_number, string errorCode, double currentDelay) {
    double currentTime = simTime().dbl(); //current time

    // Initialize logged message for log 1
    string loggedMessage = "At time " + to_string(currentTime) +
                           ", Node" + to_string(nodeID) +
                           ", Introducing channel error with code " + errorCode + ", ";
    EV << loggedMessage <<endl;
    writeFile(loggedMessage);

    // Intialize Delay with processing time delay
    double delay = packetProcessingTime+currentDelay;
    currentTime += delay;

    // create a new custom message
    Custom_message_Base *msg = new Custom_message_Base("sent");

    // set the header with the sequence number
    msg->setM_Header(seq_number);

    // Apply framing to the Payload and set it
    string framedPayload = frame(payload);

    // calculate the parity bit and add parity to the message trailer
    int parity = calcParityBit(seq_number, framedPayload);
    msg->setM_Trailer(parity);

    // introduce error to Payload
    string logger = "";
    string logger2 = "";
    int numberSent = processError(errorCode, framedPayload, logger, logger2, delay);

    // Log Message 2

    loggedMessage = "At time " + to_string(currentTime) +
                    ", Node" + to_string(nodeID) +
                    " sent frame with seq_num=" + to_string(seq_number) +
                    " and payload=" + framedPayload +
                    " and trailer=" + (bitset<8>(parity)).to_string() + ",";

    double dup_delay = duplicationDelay;

    // In case of duplicates
    string loggedMessage2 = "At time " + to_string(currentTime + dup_delay) +
                    ", Node" + to_string(nodeID) +
                    " sent frame with seq_num=" + to_string(seq_number) +
                    " and payload=" + framedPayload +
                    " and trailer=" + (bitset<8>(parity)).to_string() + ",";


    EV << loggedMessage+logger <<endl;
    writeFile(loggedMessage+logger);

    delay += transmissionDelay;

    // set the message type to data (because this is the initialization)
    msg->setM_Type(2);
    msg->setM_Payload(framedPayload.c_str());

    // set the Ack number
    msg->setM_Ack_Num(15); // needs to be properly set

    if (numberSent == 0)
        return;

    if (numberSent == 1) {
        sendDelayed(msg, SimTime(delay), "out");
    }

    if (numberSent == 2) {
        EV << loggedMessage2+logger2 <<endl;
        writeFile(loggedMessage2+logger2);
        sendDelayed(msg, SimTime(delay), "out");

        // create a new custom message
        Custom_message_Base *msgDup = new Custom_message_Base("dup");
        msgDup->setM_Header(seq_number);
        msgDup->setM_Payload(framedPayload.c_str());
        msgDup->setM_Trailer(parity);
        msgDup->setM_Type(2);
        msgDup->setM_Ack_Num(15);
        sendDelayed(msgDup, SimTime(delay + dup_delay), "out");
    }
}

void Node::handleReceive(cMessage *msg) {
    Custom_message_Base *mmsg = check_and_cast<Custom_message_Base*>(msg);
    Custom_message_Base *sentMsg = new Custom_message_Base("ACK/NACK");

    int seq_number = mmsg->getM_Header();

    if (lastReceived !=seq_number-1)
        return;

    sentMsg->setM_Header(seq_number);

    string frame = mmsg->getM_Payload();

    string payload = deframe(frame);

    int parity = mmsg->getM_Trailer();

    int par = calcParityBit(seq_number, frame);

    string AckNack = "";

    if (parity != par)
    {
        sentMsg->setM_Type(0);  // NACK
        AckNack = "NACK";
    }
    else
    {
        sentMsg->setM_Type(1);  // ACK
        AckNack = "ACK";
    }

    sentMsg->setM_Ack_Num(seq_number);

    string AckLoss = "";

    int rand = uniform(0,1)*100;
    if (rand<= probabilityOfAckLoss)
    {
        AckLoss = "Yes";
    }
    else
    {
        sendDelayed(sentMsg, SimTime(packetProcessingTime+transmissionDelay),"out");
        AckLoss = "No";
        if (sentMsg->getM_Type()==1)
        {
            lastReceived++;
            if (lastReceived == windowSize-1)
                lastReceived = -1;

        }
    }

    writeFile(to_string(simTime().dbl()));
    string loggedMessage = "At time " + to_string(simTime().dbl()+packetProcessingTime) +
                           ", Sending " + AckNack +
                           " with number " + to_string(seq_number) +
                           " , loss [" + AckLoss + " ].";

    EV << loggedMessage << endl;
    writeFile(loggedMessage);
}

void Node::writeFile(string x) {
    string outputFile = "../src/output.txt";

    // Open the file in append mode
    ofstream file(outputFile, ios::app);
    if (!file.is_open()) {
        return;
    }

    // Write the string x to the file
    file << x << endl;

    // Close the file
    file.close();
}

