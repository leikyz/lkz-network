#ifndef CLIENT_H
#define CLIENT_H

#include <string>

struct Client 
{
    int id;
    std::string ipAddress;
    int port;
    std::string lobby;

    Client(int id, const std::string& ip, int port)
        : id(id), ipAddress(ip), port(port), lobby("") {}
};

#endif // CLIENT_H
