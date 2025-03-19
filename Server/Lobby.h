#ifndef LOBBY_H
#define LOBBY_H

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include "ClientManager.h"  // Inclut le ClientManager pour pouvoir ajouter des clients

struct Lobby
{
    std::string lobbyName;
    std::unordered_map<std::string, std::shared_ptr<Client>> clients;  // Liste des clients dans le lobby

    Lobby(const std::string& name) : lobbyName(name) {}

    void addClient(const std::shared_ptr<Client>& client) {
        clients[client->ipAddress] = client;
        std::cout << "Client " << client->ipAddress << " ajouté au lobby: " << lobbyName << std::endl;
    }

    void displayClients() {
        std::cout << "Clients dans le lobby " << lobbyName << " :\n";
        for (const auto& pair : clients) {
            std::cout << "Client: " << pair.first << std::endl;
        }
    }
};

#endif // LOBBY_H
