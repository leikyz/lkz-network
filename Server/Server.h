#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>
#include "../Common/Message.h"
#include "../Common/EventManager.h"
#include "ClientManager.h"
#pragma comment(lib, "Ws2_32.lib")

#define PORT 5555 // Port d'�coute du serveur
#define BUFFER_SIZE 1024

class Server
{
private:

public:
    static void Start();
    static void Send(int clientId, Message& message);
};

#endif // SERVER_H