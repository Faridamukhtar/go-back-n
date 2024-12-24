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

#include "coordinator.h"
#include <fstream>
#include <string>
using namespace std;

Define_Module(Coordinator);

void Coordinator::initialize()
{
    int nodeId ;
    double startTime;
    readFile(nodeId, startTime);
    cMessage *msg = new cMessage(to_string(startTime).c_str());
    if (nodeId == 0)
    {
        sendDelayed(msg,SimTime(startTime) ,"out", 0);
    }
    else if (nodeId == 1)
    {
        sendDelayed(msg,SimTime(startTime), "out", 1);
    }
}

void Coordinator::handleMessage(cMessage *msg)
{
}

void Coordinator::readFile(int &nodeId, double &startTime)
{
    string inputFileName = "../src/coordinator.txt";
    std::ifstream file(inputFileName);
    if (!file.is_open())
        return;

    EV << "Reading data from file: " << inputFileName << endl;

    // Read the single line with two space-separated numbers
    if (file >> nodeId >> startTime)
    {
        cout << "Successfully read Node ID: " << nodeId << ", Start Time: " << startTime << endl;
    }

    file.close();
}
