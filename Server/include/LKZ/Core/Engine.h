#pragma once
#include "LKZ/Core/Server/INetworkInterface.h"
#include <iostream>
#include <memory>

class Engine
{
public:
    /**
	 * @brief Singleton instance accessor. Initializes with the provided network interface if not already initialized.
     * 
     * \param netInterface
     * \return 
     */
    static Engine& Instance(INetworkInterface* netInterface = nullptr) 
    {
        static Engine instance(netInterface);
        return instance;
    }

    /**
	 * @brief Main server loop that handles incoming messages and client connections.
     * 
     */
    void Run();

    /**
	 * @brief Initializes the engine and network interface.
     * 
     */
    void Initialize();

    /**
	 * @brief Accessor for the network interface.
     * 
     * \return 
     */
    static INetworkInterface* Server();

private:
    Engine(INetworkInterface* netInterface);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    static INetworkInterface* network;
};


