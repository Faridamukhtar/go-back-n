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

/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
  private :
    vector<pair<string,string>> values;
    int pointer = 0;            // to index the elements of the values vector
    bool isSender = false;  // bool to check if the current node is a sender or a receiver
  protected:
    string frame(string payload);
    string deframe(string frame);
    int calcParityBit(int seq_number, string payload);
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void readFile();
};

#endif
