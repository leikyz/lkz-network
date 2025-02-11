#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "EventManager.h"
#include "Server.h"

#pragma comment(lib, "Ws2_32.lib")

#define PORT 465 // Port d'écoute du serveur
#define BUFFER_SIZE 1024

int main() 
{
    Server::Start();

    return 0;
}
