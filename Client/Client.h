#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 465 // Port d'�coute du serveur
#define BUFFER_SIZE 1024

class Client
{
private:

public:
    static void Start();
};

#endif // CLIENT_H
