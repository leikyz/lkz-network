#pragma once
#include "INetworkInterface.h"
#include "LKZ/Core/Threading/ThreadManager.h"

#include <vector>
#include <cstdint>
#include <memory>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <mswsock.h>
#pragma comment(lib, "ws2_32.lib")

struct IoData {
    OVERLAPPED overlapped{};
    WSABUF wsabuf{};
    sockaddr_in clientAddr{};
    int clientAddrLen = sizeof(sockaddr_in);
    std::vector<uint8_t> buffer;

    IoData(size_t bufferSize) : buffer(bufferSize) {
        wsabuf.buf = reinterpret_cast<char*>(buffer.data());
        wsabuf.len = static_cast<ULONG>(buffer.size());
    }
};

class WindowsServer : public INetworkInterface {
public:
    explicit WindowsServer(int port, size_t bufferSize = 1024);
    ~WindowsServer() override;

    void Start() override;
    void Send(const sockaddr_in& clientAddr, const std::vector<uint8_t>& buffer) override;
    void Poll() override;

private:
    void InitIOCP();
    void PostReceive(IoData* ioData);
    void NotifyThreadPool(IoData* ioData, DWORD bytesTransferred);

private:
    int port;
    SOCKET listenSocket = INVALID_SOCKET;
    HANDLE completionPort = nullptr;

    std::vector<std::unique_ptr<IoData>> ioDataPool;
    size_t bufferSize;

    bool running = false;
};
