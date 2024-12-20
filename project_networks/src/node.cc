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

int Node::calcParityBit(int header, string payload)
{
    bitset<8> m(header);
    for(int i = 0; i < payload.size(); i++)
    {
         m = (m ^ bitset<8>(payload[i]));
    }
    return (int)(m.to_ulong());
}

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
    // TODO - Generated method body
    readFile();
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
        cout<<"inside sender"<<endl;
        // TODO - Generated method body
        string error_code, payload;
        if (pointer < values.size())
        {
            error_code = values[pointer].first;
            payload = values[pointer].second;
            pointer++;
        }
        // create a new custom message
        Custom_message_Base * mmsg = new Custom_message_Base();

        // set the header with the sequence number
        int seq_number = pointer - 1; // to be initialized properly later
        mmsg->setM_Header(seq_number);

        // apply framing to the payload and set it
        string framed = frame(payload);
        cout<<"framed payload = "<<framed<<endl;
        mmsg->setM_Payload(framed.c_str());

        // calculate the parity bit
        int parity = calcParityBit(seq_number, framed);

        // add parity to the message trailer
        mmsg->setM_Trailer(parity);

        // set the message type to data (because this is the initialization)
        mmsg-> setM_Type(2);

        // set the ack number
        mmsg-> setM_Ack_Num(15); // needs to be properly set

        cout<<"the sender sent message with a header : "<<mmsg->getM_Header()<<" ,payload : "<<mmsg->getM_Payload()<<
           " ,trailer : "<<mmsg->getM_Trailer()<<", and type : "<<mmsg->getM_Type()<<endl;

       send(mmsg,"out");
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

