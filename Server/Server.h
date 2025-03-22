#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include "ClientManager.h"
#include <winsock2.h> 
#include "Message.h"
#include "LobbyManager.h" 
#pragma comment(lib, "Ws2_32.lib")

#define PORT 5555
#define BUFFER_SIZE 1024

class Server
{
private:
    static SOCKET serverSocket;

public:
    static void Start();
    static void Send(sockaddr_in clientAddr, const std::vector<uint8_t>& buffer);

    static void SendToAllInLobby(std::shared_ptr<Lobby>, const std::vector<uint8_t>& buffer);

    static void SendToAllInLobbyExcept(std::shared_ptr<Lobby>, const sockaddr_in& excludedClientAddr, const std::vector<uint8_t>& buffer);

    static void SendToAllClients(const std::vector<uint8_t>& buffer);

    static void SendToAllClientsExcept(const sockaddr_in& excludedClientAddr, const std::vector<uint8_t>& buffer);
};

#endif // SERVER_H
