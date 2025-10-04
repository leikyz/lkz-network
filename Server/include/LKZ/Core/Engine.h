#include "LKZ/Core/Server/INetworkInterface.h"
#include <iostream>

class Engine 
{

public:
    Engine(INetworkInterface* netInterface)
        : network(netInterface) {
    }

    void Run();
      
private:
    INetworkInterface* network;
};
