#include <string>
#include <memory>

struct Client 
{
    int id;
    sockaddr_in address;
    std::string ipAddress;
};
