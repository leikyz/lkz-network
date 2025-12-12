#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

namespace Protocol {
    enum PacketID : uint8_t { Handshake = 255, TestMessage = 1, FrameData = 2 };
}

struct GameStats
{
    float currentDeltaTime = 0.0f;
    std::vector<float> dtHistory;

    uint64_t totalPackets = 0;
};

class ProfilerClient
{
public:
    ProfilerClient();
    ~ProfilerClient();

    bool Connect(const std::string& ip, int port);
    void Disconnect();
    void Update();

    const GameStats& GetStats() const { return stats; }
    bool IsConnected() const { return connected; }

    float GetTimeSinceLastPacket() const;

private:
    void ProcessPacket(std::vector<uint8_t>& data);

    SOCKET clientSocket = INVALID_SOCKET;
    sockaddr_in serverAddr{};
    bool connected = false;

    GameStats stats;

    std::chrono::steady_clock::time_point lastHandshakeTime;

    std::chrono::steady_clock::time_point lastPacketTime;
};