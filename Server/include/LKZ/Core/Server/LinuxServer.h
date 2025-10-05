#ifndef LINUX_SERVER_H
#define LINUX_SERVER_H

#include "LKZ/Core/Server/INetworkInterface.h"  
#include <vector>  

class LinuxServer : public INetworkInterface 
{
public:
    LinuxServer(int port);
    ~LinuxServer();

    void Start() override;
    void Send(const Client& client, const std::vector<uint8_t>& buffer) override;
    void Poll() override;

private:
    int port;
    SOCKET listenSocket;
};

#endif // LINUX_SERVER_H