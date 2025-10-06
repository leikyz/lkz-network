#include "LKZ/Core/Manager/ClientManager.h"

std::unordered_map<std::string, Client*> ClientManager::clients;
std::mutex ClientManager::clientsMutex;
std::atomic<uint32_t> ClientManager::nextId{ 1 }; // For avoiding multithreading issues

/**
 * @brief Adds a new client to the manager if it doesn't already exist safe.
 * @param clientAddr The address of the client to add.
 */

void ClientManager::addClient(const sockaddr_in& clientAddr)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    std::string key = getClientKey(clientAddr);
    if (clients.find(key) == clients.end())
    {
        clients[key] = new Client(nextId.fetch_add(1), clientAddr, key); 
    }
}

/**
 * @brief Removes a client from the manager by its address safe.
 * @param clientAddr The address of the client to remove.
 */

void ClientManager::removeClient(const sockaddr_in& clientAddr)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    std::string key = getClientKey(clientAddr);
    auto it = clients.find(key);
    if (it != clients.end())
    {
        delete it->second;
        clients.erase(it);
    }
}
Client* ClientManager::getClientByAddress(const sockaddr_in& clientAddr)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    std::string key = getClientKey(clientAddr);
    auto it = clients.find(key);
    return (it != clients.end()) ? it->second : nullptr;
}

std::vector<Client*> ClientManager::getClients()
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    std::vector<Client*> clientList;
    for (auto& pair : clients)
    {
        clientList.push_back(pair.second);
    }
    return clientList;
}

std::string ClientManager::getClientKey(const sockaddr_in& clientAddr)
{
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
    return std::string(ipStr) + ":" + std::to_string(ntohs(clientAddr.sin_port));
}

