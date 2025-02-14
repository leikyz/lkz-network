#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <winsock2.h>
#include <iostream>
#include "Client.h"  // Assure-toi que la structure Client est incluse

class ClientManager
{
public:

    /*static int nextId;*/
    // Ajouter un client avec son adresse IP
    static void addClient(const std::string& ipAddress, SOCKET sock);

    // Rechercher un client par ID
    static std::shared_ptr<Client> getClientById(int clientId);

    // Afficher tous les clients
    static void displayClients();

private:
    static std::unordered_map<int, std::shared_ptr<Client>> clients;  // Map statique pour stocker les clients par leur ID
};

#endif // CLIENT_MANAGER_H
