#include <string>
#include <memory>

struct Client 
{
    int id;
    sockaddr_in address;
    SOCKET socket;
    std::string ipAddress;
};
