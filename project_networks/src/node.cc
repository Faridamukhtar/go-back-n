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

string Node:: frame(string payload)
{
    string s;
    s+= '$';
    for (int i = 0; i < payload.length(); i++)
    {
        if (payload[i] == '$' || payload[i] == '/')
            s += '/';
        s+= payload[i];
    }
    s+= '$';
    return s;
}

string Node:: deframe(string frame)
{
    string payload;
    for (int i = 1; i < frame.length()-1; i++)
    {
        if (frame[i] == '/' && (frame[i+1] == '/' || frame[i+1] == '$'))
        {
            payload += frame[i+1];
            i++;
        }
        else
            payload += frame[i];
    }
    return payload;
}

// INITIALIZE LOGGERS WITH EMPTY STRINGS
int Node::processError(string errorCode, string& framedPayload, string &logger, string& logger2, int & delay)
{
    int numberSent = 1; // number of packets sent [0 for loss, 2 for duplicates]
    int errorDelay = 4; //TODO: REPLACE WITH ERROR DELAY

    // Modified
    if (errorCode[0]=='1')
    {
        // Modification Logic: Modify 9th bit (1st bit in second byte)
        bitset<8> char1 = bitset<8>(framedPayload[1]);
        char1[0]= not char1[0];
        framedPayload[1] = char1.to_string();

        // Log Modified Bit
        logger+="Modified [9] ,";

    }
    else
    {
        // Log No Modification
        logger+="Modified [-1] ,";
    }

    //loss
    if (errorCode[1]=='1')
    {
        // indicate no sent packet (LOSS)
        numberSent = 0;

        // Log Loss
        logger+= "Lost [Yes] ,";
    }
    else
    {
        // Log No Loss
        logger+= "Lost [No] ,";
    }

    // duplicate
    if (errorCode[2]=='1')
    {
        // Log Duplicate (Both Frames)
        logger2+= logger;
        logger+= "Duplicate [1] ,";
        logger2+= "Duplicate [2] ,";

        // carry out duplicate logic (+0.1 (DUP) delay)

        // indicate 2 sent packets if NO LOSS (DUPLICATE)
        if(errorCode[1]=='0')
            numberSent = 2;
    }
    else
    {
        // Log No Duplicate
        logger+= "Duplicate [0] ,";
    }

    //delay
    if (errorCode[3]=='1')
    {
        logger+= "Delay [" + to_string(errorDelay) + "].";
        // Log error to duplicate packet
        if(errorCode[2]=='1')
        {
            logger2+= "Delay [" + to_string(errorDelay) + "].";
        }
        delay+= errorDelay;
        // carry out delay logic (+ delay error (ED) stored fel init)
    }
    else
    {
        logger+= "Delay [0] ,";
        if(errorCode[2]=='1')
        {
            logger2+= "Delay [0].";
        }
    }
}

int Node::calcParityBit(int header, string payload)
{
    bitset<8> m(header);
    for(int i = 0; i < payload.size(); i++)
    {
         m = (m ^ bitset<8>(payload[i]));
    }
    return (int)(m.to_ulong());
}

void Node::initializeSender() {
    seqNumber = 0;
    expectedSeqNumber = 0;
    windowSize = par("windowSize").intValue();  // Set from .ini file
    base = 0;
    timer = nullptr;
    string errorCode = "1001"; //TODO: from file

    // Start sending
    if (!queue.empty()) {
        handleSend(queue.front(), seqNumber);
    }
}

// start i at 0
// haneb3at n messages ( 3ala 7asab el windowSize) starting men i le el max(len(vector beta3 el messages, i+window size)
// hayerga3ly acks
// ha keep track of max ack within el time frame el ana feeh
// le7ad el timeout hafdal mestaneya akbar rakam ack haygeely
// law akbar ack <= (window size -1) sa3etha ha resend packets from akbar packet le window size -1
// lama el ack el sa7 yewsal -> sa3etha ha increment el i to window size
// repeat


void Node ::readFile()
{
    string inputFileName = "D:/uni_courses/sem7/networks-1/go-back-N-DLL/project_networks/src/input0.txt";
        std::ifstream file(inputFileName);
        if (!file.is_open())
        {
            cout << "Error: Could not open file " << inputFileName << endl;
            return;
        }

    cout << "Reading data from file: " << inputFileName << endl;

    std::string line;
    string id, description;
    while (std::getline(file, line))
    {
        id = line.substr(0, 4);
        description = line.substr(5);
        values.push_back({id,description});
        cout<<"id = "<<id<<" , description"<<description<<endl;
    }

    file.close();
}

void Node::initialize()
{
    readFile();

    windowSize = par("WS").intValue();
    timeout = par("TO").doubleValue();
    packetProcessingTime = par("PT").doubleValue();
    transmissionDelay = par("TD").doubleValue();
    errorDelayTime = par("ED").doubleValue();
    duplicationDelay = par("DD").doubleValue();
    probabilityOfAckLoss = par("LP").intValue();
}

void Node::handleMessage(cMessage *msg)
{
    // Get the gate through which the message was received
    cGate* arrivalGate = msg->getArrivalGate();

    // You can access the gate's index to check which gate it arrived at
    int gateIndex = arrivalGate->getIndex();

    // received a message from the coordinator --> sender
    if (gateIndex == 1 && isSender == false)
    {
        isSender = true;
    }

    // sender handling
    if (isSender)
    {
        string error_code, payload;
        if (pointer < values.size())
        {
            error_code = values[pointer].first;
            payload = values[pointer].second;
            pointer++;
        }

        handleSend(payload, pointer - 1, error_code);
    }

    //receiver handling
    else
    {
        cout<<"inside receiver"<<endl;
        Custom_message_Base * mmsg = check_and_cast<Custom_message_Base *>(msg);
        string frame = mmsg->getM_Payload();
        string payload = deframe(frame);
        cout<<"after deframing : "<<payload<<endl;
        int parity = mmsg->getM_Trailer();
        int par = calcParityBit(mmsg->getM_Header(), frame);
        if (parity != par)
        {
            mmsg->setM_Type(0);  // nack
            cout<<"error in parity"<<endl;
        }
        else
        {
            mmsg->setM_Type(1);  // ack
            cout<<"correct parity"<<endl;
        }
        mmsg->setM_Ack_Num(mmsg->getM_Header());
        send(mmsg,"out");
    }
}

void Node::handleSend(string payload, int seq_number, string errorCode)
{
    double currentTime = SimTime(); //current time

    //  set node id
    const char *nodeName = getName();
    int nodeID = -1;
    if (strcmp(nodeName, "node2") == 0)
        nodeID = 1;
    else
        nodeID = 0;

    // Initialize logged message for log 1
    string loggedMessage = format("At time [{}], Node[{}], Introducing channel error with code {}, ",
    currentTime, nodeID, errorCode);

    // TODO: LOG MESSAGE TO OUTPUT FILE

    // Intialize Delay with processing time delay
    double delay = packetProcessingTime;
    currentTime +=delay;

    // create a new custom message
    Custom_message_Base * msg = new Custom_message_Base();

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
    int numberSent = processError(errorCode, framedPayload, logger, logger2, delay);

    // Initialize logged message for log 2
    loggedMessage = format("At time [{}], Node[{}] sent frame with seq_num=[{}] and payload=[{}] and trailer=[{}],",
    currentTime, nodeID, seq_number, framedPayload, (bitset<8>(parity)).to_string());

    double dup_delay = duplicationDelay;

    // In case of duplicates
    string loggedMessage2 = format("At time [{}], Node[{}] sent frame with seq_num=[{}] and payload=[{}] and trailer=[{}],",
    (currentTime+dup_delay), nodeID, seq_number, framedPayload, (bitset<8>(parity)).to_string());

    // TODO: log loggedMessage 1

    delay+=transmissionDelay; // TODO: MOVE TO INI (transmission/channel delay)

    // set the message type to data (because this is the initialization)
    msg-> setM_Type(2);

    // set the Ack number
    msg-> setM_Ack_Num(15); // needs to be properly set

    if (numberSent == 0)
        return;

    if (numberSent == 1)
    {
        sendDelayed(msg, delay, "out");
    }

    if (numberSent == 2)
    {
        // TODO: log loggedMessage 2
        sendDelayed(msg, delay, "out");
        sendDelayed(msg, (delay+dup_delay), "out");
    }

}
