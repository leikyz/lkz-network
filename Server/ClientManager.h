#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <winsock2.h>
#include <iostream>
#include "Client.h"  
#include <ws2tcpip.h>
#include <stdio.h>

class ClientManager
{
public:
    static void addClient(sockaddr_in clientAddr);
    static Client* getClientByAddress(const sockaddr_in& clientAddr);
    static std::vector<Client*> getClients();
    static void removeClient(const sockaddr_in& clientAddr);

private:
    static std::unordered_map<std::string, Client*> clients;

    static std::string getClientKey(const sockaddr_in& clientAddr);
};

#endif // CLIENT_MANAGER_H
