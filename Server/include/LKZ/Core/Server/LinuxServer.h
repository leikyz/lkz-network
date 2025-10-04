#pragma once  
#include "LKZ/Core/Server/INetworkInterface.h"  
#include <vector>  


class LinuxServer : public INetworkInterface {
public:
    LinuxServer(int port);
    ~LinuxServer();

    void Start() override;
    void Send(const sockaddr_in& clientAddr, const std::vector<uint8_t>& buffer) override;
    void Poll() override;

private:
    int port;
    SOCKET listenSocket;
};
