#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP "127.0.0.1" // Adresse du serveur (localhost)
#define SERVER_PORT 465       // Port du serveur
#define BUFFER_SIZE 1024

int main() {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        printf("Erreur lors de l'initialisation de Winsock\n");
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erreur lors de la création du socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr); // Convertit l'IP en format réseau

    char buffer[BUFFER_SIZE] = "Hello, serveur UDP!";
    int sendResult = sendto(clientSocket, buffer, strlen(buffer), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sendResult == SOCKET_ERROR) {
        printf("Erreur lors de l'envoi du message: %d\n", WSAGetLastError());
    }
    else {
        printf("Message envoyé au serveur: %s\n", buffer);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
