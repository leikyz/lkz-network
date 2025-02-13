#include "Client.h"
#include "../Common/EventManager.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // Adresse IP locale
#define PORT 5555
#define BUFFER_SIZE 1024

void Client::Start()
{
    /*std::cout << "test";*/

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Erreur d'initialisation de Winsock" << std::endl;
    }

    // Cr�ation du socket UDP
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Erreur lors de la cr�ation du socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
    }

    // Pr�paration de l'adresse du serveur
   // Pr�paration de l'adresse du serveur avec inet_pton
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) != 1) {
        std::cerr << "Erreur de conversion de l'adresse IP" << std::endl;
        WSACleanup();
        return;
    }


    // S�rialisation de l'entier 10
    int value = 10;
    std::vector<int8_t> serializedData = EventManager::Serialize(value);

    // Envoi des donn�es au serveur
    int sendResult = sendto(clientSocket, reinterpret_cast<const char*>(serializedData.data()), serializedData.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "Erreur d'envoi: " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "Entier 10 envoy� au serveur." << std::endl;
    }

    // Fermeture du socket
    closesocket(clientSocket);
    WSACleanup();
}