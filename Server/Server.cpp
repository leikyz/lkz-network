﻿#include "Server.h"
#include "EventManager.h"
#include "Message.h"

SOCKET Server::serverSocket = INVALID_SOCKET;

void Server::Start()
{
    EventManager::BindEvents();

    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        printf("Error during the Initialization of Winsock \n");
        return;
    }

    // Create UDP Socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) 
    {
        printf("Erreur lors de la création du socket: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Create Server
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET; // IPV4
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT); // Convert port to network format

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) 
    {
        printf("Échec du bind: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    printf("UDP Server listening on port %d...\n", PORT);

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    while (true)
    {
        int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) 
        {
            printf("Message receive error: %d\n", WSAGetLastError());
            break;
        }

        std::vector<uint8_t> receivedData(buffer, buffer + bytesReceived);

        int id = static_cast<int>(buffer[0]);

        // To create client
        if (id == 1)
        {
            char ipAddress[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, INET_ADDRSTRLEN);
            unsigned short port = ntohs(clientAddr.sin_port);

            ClientManager::addClient(clientAddr);
            EventManager::processMessage(receivedData, clientAddr);
        }
        else
        {
            EventManager::processMessage(receivedData, clientAddr);
        }
    }
    closesocket(serverSocket);
    WSACleanup();
}


// Send function to a specific client
void Server::Send(sockaddr_in clientAddr, const std::vector<uint8_t>& buffer)
{
    auto client = ClientManager::getClientByAddress(clientAddr);
    if (!client)
    {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);

        std::cerr << "Client not found"
            << ipStr << ":"
            << ntohs(clientAddr.sin_port) << std::endl;

        return;
    }

    // Envoi du message sérialisé sous forme de buffer
    int bytesSent = sendto(serverSocket, reinterpret_cast<const char*>(buffer.data()), buffer.size(), 0,
        reinterpret_cast<const sockaddr*>(&client->address), sizeof(client->address));

    if (bytesSent == SOCKET_ERROR)
        std::cerr << "Erreur lors de l'envoi du message: " << WSAGetLastError() << std::endl;
    else
    {
        const char* purpleColor = "\033[38;5;129m";
        std::cout << purpleColor << "(" << client->ipAddress << ") Message sent: {"
            << buffer.size() << " bytes}" << std::endl;
    }
}


