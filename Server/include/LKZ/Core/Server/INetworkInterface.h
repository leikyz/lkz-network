#pragma once
#include <vector>
#include <cstdint>

// Platform-specific includes for socket programming
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#include <windows.h>  
#include <mswsock.h>  
    #pragma comment(lib, "ws2_32.lib")
#else
    //  Unix/Linux
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <sys/epoll.h>
#endif

class INetworkInterface 
{
public:
    virtual ~INetworkInterface() {}

    // Start the server
    virtual void Start() = 0;

    // Send a packet
    virtual void Send(const sockaddr_in& clientAddr, const std::vector<uint8_t>& buffer) = 0;

    // Poll for incoming packets
    virtual void Poll() = 0;
};
