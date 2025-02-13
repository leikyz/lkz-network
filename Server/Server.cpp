#include "Server.h"
#include "../Common/EventManager.h"

void Server::Start()
{
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        printf("Error during the Initialization of Winsock \n");
        return;
    }

    // Create UDP Socket
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Erreur lors de la cr�ation du socket: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Create Server
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET; // IPV4
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT); // Convert port to network format

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("�chec du bind: %d\n", WSAGetLastError());
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
        if (bytesReceived == SOCKET_ERROR) {
            printf("Message receive error: %d\n", WSAGetLastError());
            break;
        }

        // Convertir les donn�es re�ues en vecteur de bytes
        std::vector<int8_t> receivedData(buffer, buffer + bytesReceived);

        // D�s�rialiser et afficher le r�sultat
        EventManager::Deserialize(receivedData);
    }

    closesocket(serverSocket);
    WSACleanup();
}