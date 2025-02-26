#include "ClientManager.h"

std::unordered_map<int, std::shared_ptr<Client>> ClientManager::clients;
int ClientManager::nextId = 1; 

void ClientManager::addClient(sockaddr_in clientAddr, SOCKET socket, const std::string& ipAddress)
{
    int clientId = nextId++; 

    auto client = std::make_shared<Client>();
    client->id = clientId;
    client->address = clientAddr;
    client->socket = socket;
    client->ipAddress = ipAddress;
    clients[clientId] = client;

    std::cout << "Client added : ID=" << clientId << ", IP=" << ipAddress << std::endl;
}

std::shared_ptr<Client> ClientManager::getClientById(int clientId)
{
    auto it = clients.find(clientId);
    if (it != clients.end()) {
        return it->second;
    }
    return nullptr;  
}

void ClientManager::displayClients()
{
    for (const auto& pair : clients) {
        std::cout << "Client ID: " << pair.second->id << ", IP: " << pair.second->ipAddress << std::endl;
    }
}
