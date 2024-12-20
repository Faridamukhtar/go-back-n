package project_networks;

import ned.IdealChannel;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=745,299");
    submodules:
        node1: Node {
            @display("p=114,88");
        }
        node2: Node {
            @display("p=434,89");
        }
        coordinator: Coordinator {
            @display("p=286,193");
        }
    connections:
        node1.out --> IdealChannel --> node2.in[0];
        node2.out --> IdealChannel --> node1.in[0];
        coordinator.out[0] --> IdealChannel --> node1.in[1];
        coordinator.out[1] --> IdealChannel --> node2.in[1];
}

