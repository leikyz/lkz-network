#include "ClientManager.h"

std::unordered_map<std::string, Client*> ClientManager::clients;

std::string ClientManager::getClientKey(const sockaddr_in& clientAddr)
{
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
    return std::string(ipStr) + ":" + std::to_string(ntohs(clientAddr.sin_port));
}

void ClientManager::addClient(sockaddr_in clientAddr)
{
    std::string key = getClientKey(clientAddr);

    if (clients.find(key) != clients.end()) {
        std::cout << "Client already exists: " << key << std::endl;
        return;
    }

    Client* client = new Client();
    client->address = clientAddr;
    client->ipAddress = key;

    clients[key] = client;
}

void ClientManager::removeClient(const sockaddr_in& clientAddr)
{
    std::string key = getClientKey(clientAddr);
    auto it = clients.find(key);
    if (it != clients.end()) {
        delete it->second;        // ⚠️ très important : libérer la mémoire
        clients.erase(it);
    }
}

Client* ClientManager::getClientByAddress(const sockaddr_in& clientAddr)
{
    std::string key = getClientKey(clientAddr);
    auto it = clients.find(key);
    if (it != clients.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<Client*> ClientManager::getClients()
{
    std::vector<Client*> clientList;
    clientList.reserve(clients.size());
    for (const auto& pair : clients) {
        clientList.push_back(pair.second);
    }
    return clientList;
}
