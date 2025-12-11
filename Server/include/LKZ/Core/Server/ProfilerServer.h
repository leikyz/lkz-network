#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <memory>
#include <mutex>

struct ProfilerIoData
{
    OVERLAPPED overlapped{};
    WSABUF wsabuf{};
    sockaddr_in clientAddr{};
    int clientAddrLen = sizeof(sockaddr_in);
    std::vector<uint8_t> buffer;

    ProfilerIoData(size_t bufferSize) : buffer(bufferSize) {
        wsabuf.buf = reinterpret_cast<char*>(buffer.data());
        wsabuf.len = static_cast<ULONG>(buffer.size());
    }
};

class ProfilerServer
{
public:
    explicit ProfilerServer(int port);
    ~ProfilerServer();

    void Start();
    void Stop();

    void Poll();

    void Broadcast(const std::vector<uint8_t>& data);

private:
    void InitIOCP();
    void PostReceive(ProfilerIoData* ioData);
    void HandlePacket(ProfilerIoData* ioData, DWORD bytesTransferred);
    void SendInternal(const sockaddr_in& target, const std::vector<uint8_t>& buffer);

    int port;
    SOCKET listenSocket = INVALID_SOCKET;
    HANDLE completionPort = nullptr;
    bool running = false;

    std::vector<std::unique_ptr<ProfilerIoData>> ioDataPool;

    std::vector<sockaddr_in> subscribers;
    std::mutex subscribersMutex;
};