
# Go-Back-N Protocol - Data Link Layer Implementation

This repository contains an implementation of the **Go-Back-N Automatic Repeat reQuest (ARQ) Protocol** at the **Data Link Layer** of the OSI model. 
The Go-Back-N protocol is a sliding window mechanism used to ensure reliable data transmission by handling packet loss, retransmissions, and maintaining proper sequencing.

## Features

- **Go-Back-N ARQ**: Implements a reliable data transmission protocol at the Data Link Layer.
- **Sliding Window Mechanism**: Ensures efficient transmission with support for multiple frames in transit.
- **Error Handling**: Handles packet loss and ensures retransmission of lost or corrupted frames.
- **Simplified Design**: Focuses on the key functionality of the Go-Back-N protocol.

## Requirements
- **Operating System**: Tested on Windows and Unix-like systems.
- **Build Environment**: omnetpp

## Installation

### Build and run the Project

1. Clone the repository:
   ```bash
   git clone https://github.com/Faridamukhtar/go-back-N-DLL.git
   cd go-back-N-DLL
   ```
2. Run omnetpp
3. Build project
4. run simulation in omnetpp
