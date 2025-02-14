#include <string>
#include <memory>

struct Client
{
    int clientId;
    std::string ipAddress;
    SOCKET socket;  // Le socket correspondant au client

    // Constructeur
    Client(const std::string& ip, SOCKET sock)
        : clientId(1), ipAddress(ip), socket(sock) {}  // Le client reçoit un ID unique et un socket
};
