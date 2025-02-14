#include "Client.h"
#include "../Common/EventManager.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // Adresse IP locale
#define PORT 5555
#define BUFFER_SIZE 1024

void Client::Start()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Erreur d'initialisation de Winsock" << std::endl;
        return;
    }

    // Création du socket UDP
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Erreur lors de la création du socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) != 1) {
        std::cerr << "Erreur de conversion de l'adresse IP" << std::endl;
        WSACleanup();
        return;
    }

    // Sérialisation de l'entier
    int value = 1;
    std::vector<uint8_t> serializedData = EventManager::serialize(value);

    // Sérialisation de l'adresse IP
    std::string ipAddress = "192.168.1.1";  // Exemple d'adresse IP
    std::vector<int8_t> serializedIp(ipAddress.begin(), ipAddress.end());
    serializedIp.push_back('\0');  // Ajouter un séparateur de fin de chaîne

    // Concaténer les deux sérialisations (l'entier et l'adresse IP) dans un seul buffer
    std::vector<int8_t> finalData;
    finalData.insert(finalData.end(), serializedData.begin(), serializedData.end());
    finalData.insert(finalData.end(), serializedIp.begin(), serializedIp.end());

    // Envoi des données au serveur
    int sendResult = sendto(clientSocket, reinterpret_cast<const char*>(finalData.data()), finalData.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sendResult == SOCKET_ERROR) {
        std::cerr << "Erreur d'envoi: " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "Entier et adresse IP envoyés au serveur." << std::endl;
    }

    // Fermeture du socket
    closesocket(clientSocket);
    WSACleanup();
}
