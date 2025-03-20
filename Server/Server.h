#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include "ClientManager.h"
#include <winsock2.h> 
#include "Message.h"
#include "LobbyManager.h"  // Pour la gestion des lobbies
#pragma comment(lib, "Ws2_32.lib")

#define PORT 5555
#define BUFFER_SIZE 1024

class Server
{
private:
    static SOCKET serverSocket;

public:
    static void Start();
    void Send(sockaddr_in clientAddr, const std::vector<uint8_t>& buffer);

    // Nouvelle fonction pour envoyer à tous les clients d'un lobby
    void SendToAllInLobby(int lobbyId, const std::vector<uint8_t>& buffer);

    // Nouvelle fonction pour envoyer à tous les clients d'un lobby sauf un
    void SendToAllInLobbyExcept(int lobbyId, const sockaddr_in& excludedClientAddr, const std::vector<uint8_t>& buffer);

    // Nouvelle fonction pour envoyer à tous les clients
    void SendToAllClients(const std::vector<uint8_t>& buffer);

    // Nouvelle fonction pour envoyer à tous les clients sauf un
    void SendToAllClientsExcept(const sockaddr_in& excludedClientAddr, const std::vector<uint8_t>& buffer);
};

#endif // SERVER_H
