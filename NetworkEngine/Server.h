#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>
#include "EventManager.h"
#include "Client.h"  

#pragma comment(lib, "Ws2_32.lib")

#define PORT 465 // Port d'écoute du serveur
#define BUFFER_SIZE 1024

class Server
{
private:
    static std::vector<Client> clients; // Liste des clients connectés

public:
    static void Start();
    static void AddClient(const Client& client); 
    static const std::vector<Client>& GetClients();  
};

#endif // SERVER_H
