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

int Node::calcParityBit(int header, string payload)
{
    bitset<8> m(header);
    for(int i = 0; i < payload.size(); i++)
    {
         m = (m ^ bitset<8>(payload[i]));
    }
    return (int)(m.to_ulong());
}

void Node::initialize()
{
    // TODO - Generated method body

    // take the payload from the user
    string payload;
    cin>> payload;

    // create a new custom message
    Custom_message_Base * msg = new Custom_message_Base();

    // set the header with the sequence number
    int seq_number = 36; // to be initialized properly later
    msg->setM_Header(seq_number);

    // apply framing to the payload and set it
    string framed = frame(payload);
    cout<<"framed payload = "<<framed<<endl;
    msg->setM_Payload(framed.c_str());

    // calculate the parity bit
    int parity = calcParityBit(seq_number, framed);

    // add parity to the message trailer
    msg->setM_Trailer(parity);

    // set the message type to data (because this is the initialization)
    msg-> setM_Type(2);

    // set the ack number
    msg-> setM_Ack_Num(15); // needs to be properly set

    cout<<"the sender sent message with a header : "<<msg->getM_Header()<<" ,payload : "<<msg->getM_Payload()<<
       " ,trailer : "<<msg->getM_Trailer()<<", and type : "<<msg->getM_Type()<<endl;

   send(msg,"out");
}

void Node::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
