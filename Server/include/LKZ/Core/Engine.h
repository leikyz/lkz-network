#pragma once
#include "LKZ/Core/Server/INetworkInterface.h"
#include <iostream>
#include <memory>

class Engine
{
public:
    static Engine& Instance(INetworkInterface* netInterface = nullptr) {
        static Engine instance(netInterface);
        return instance;
    }

    void Run();
    void Initialize();
    static INetworkInterface* Server();

private:
    Engine(INetworkInterface* netInterface);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    static INetworkInterface* network;
};


