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

#ifndef __PROJECT_NETWORKS_NODE_H_
#define __PROJECT_NETWORKS_NODE_H_

#include <omnetpp.h>

using namespace omnetpp;
using namespace std;

class Node : public cSimpleModule
{
    private :
    double timeout;                      // Time (in seconds) to wait for an acknowledgment before retransmitting packets
    double packetProcessingTime;         // Time (in seconds) required for the receiver to process a packet
    double transmissionDelay;            // Simulated time (in seconds) for a packet to travel from sender to receiver
    double errorDelayTime;               // Additional delay (in seconds) introduced due to network errors (e.g., congestion)
    double duplicationDelay;             // Delay (in seconds) added to simulate duplicate packets in the network
    double probabilityOfAckLoss;         // Probability (0 to 1) that an acknowledgment (ACK) is lost during transmission

    int seqNumber;                       // Current sequence number of the packet being sent by the sender
    int expectedSeqNumber;               // The sequence number the receiver expects to receive next
    int windowSize;                      // The maximum number of packets the sender can send without waiting for an acknowledgment
    int base;                            // The sequence number of the oldest unacknowledged packet (start of the sender's sliding window)

    queue<string> queue;                 // Queue to store payloads (messages) that the sender needs to transmit
    vector<pair<string, string>> values; // A vector of <error type, message> pairs to simulate errors in specific packets
    int pointer = 0;                     // Index for iterating through the `values` vector, used for error simulation
    bool isSender = false;               // Boolean flag indicating whether this node is acting as a sender (true) or receiver (false)

  protected:
    virtual int processError(string errorCode, string& framedPayload, string &logger, string& logger2);
    virtual int calcParityBit(int seq_number, string payload);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void handleSend();
    void handleAck(int ackNumber);
    void handleTimeout();
    string frame(string payload);
    string deframe(string frame);
    void readFile();
};

#endif
