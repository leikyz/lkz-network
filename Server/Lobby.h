#ifndef LOBBY_H
#define LOBBY_H

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include <list> 
#include "ClientManager.h"  

struct Lobby
{
    std::string lobbyName;
    std::list<std::shared_ptr<Client>> clients;  

    Lobby(const std::string& name) : lobbyName(name) {}

    void addClient(const std::shared_ptr<Client>& client) {
        clients.push_back(client);  
        std::cout << "Client " << client->ipAddress << " ajouté au lobby: " << lobbyName << std::endl;
    }

    std::shared_ptr<Client> getClientByIp(const std::string& ipAddress) const
    {
        for (const auto& client : clients)
        {
            if (client->ipAddress == ipAddress) 
            {
                return client;  
            }
        }
        return nullptr; 
    }

    std::list<std::shared_ptr<Client>> getClients() const
    {
        return clients;
    }
};

#endif // LOBBY_H
