#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include <winsock2.h>
#include <iostream>
#include "Client.h"  

class ClientManager
{
public:
    static void addClient(sockaddr_in clientAddr, SOCKET socket, const std::string& ipAddress);
    static std::shared_ptr<Client> getClientById(int clientId);
    static void displayClients();

private:
    static std::unordered_map<int, std::shared_ptr<Client>> clients;
    static int nextId;
};

#endif // CLIENT_MANAGER_H
