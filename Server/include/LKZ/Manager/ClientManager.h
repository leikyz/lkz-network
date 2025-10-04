#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "LKZ/NetworkObject/Client.h"

class ClientManager {
public:
    static void addClient(const sockaddr_in& clientAddr);
    static Client* getClientByAddress(const sockaddr_in& clientAddr);
    static std::vector<Client*> getClients();
    static void removeClient(const sockaddr_in& clientAddr);

private:
    static std::unordered_map<std::string, Client*> clients;
    static std::mutex clientsMutex;

    static std::atomic<uint32_t> nextId; // Thread-safe 

    static std::string getClientKey(const sockaddr_in& clientAddr);
};

#endif // CLIENT_MANAGER_H
