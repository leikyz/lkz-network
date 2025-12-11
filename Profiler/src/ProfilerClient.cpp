#include <iostream>
#include <vector>
#include <Profiler/include/ProfilerClient.h>

ProfilerClient::ProfilerClient()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    stats.dtHistory.reserve(1000);
}

ProfilerClient::~ProfilerClient()
{
    Disconnect();
    WSACleanup();
}

bool ProfilerClient::Connect(const std::string& ip, int port)
{
    if (clientSocket != INVALID_SOCKET) Disconnect();

    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) return false;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    u_long mode = 1;
    ioctlsocket(clientSocket, FIONBIO, &mode);

    uint8_t handshakeID = Protocol::Handshake;
    sendto(clientSocket, (char*)&handshakeID, 1, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    connected = true;
    lastHandshakeTime = std::chrono::steady_clock::now();
    return true;
}

void ProfilerClient::Disconnect()
{
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
    connected = false;
}

void ProfilerClient::Update()
{
    if (!connected || clientSocket == INVALID_SOCKET) return;

    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - lastHandshakeTime).count() > 2)
    {
        uint8_t handshakeID = Protocol::Handshake;
        sendto(clientSocket, (char*)&handshakeID, 1, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
        lastHandshakeTime = now;
    }

    char buffer[4096];
    sockaddr_in sender;
    int senderLen = sizeof(sender);

    while (true)
    {
        int bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0, (sockaddr*)&sender, &senderLen);

        if (bytesReceived == SOCKET_ERROR) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
            }
            break;
        }

        if (bytesReceived > 0) {
            std::vector<uint8_t> packet(buffer, buffer + bytesReceived);
            ProcessPacket(packet);
        }
    }
}

void ProfilerClient::ProcessPacket(const std::vector<uint8_t>& data)
{
    if (data.empty()) return;

    stats.totalPackets++;
    lastPacketTime = std::chrono::steady_clock::now();

    uint8_t id = data[0];

    switch (id)
    {
    case Protocol::TestMessage:
        if (data.size() >= 5)
        {
            int val = *reinterpret_cast<const int*>(&data[1]);
            float dt = static_cast<float>(val) / 1000.0f;
            stats.currentDeltaTime = dt;
            stats.dtHistory.push_back(dt);
            if (stats.dtHistory.size() > 500) {
                stats.dtHistory.erase(stats.dtHistory.begin());
            }
        }
        break;
    }
}

float ProfilerClient::GetTimeSinceLastPacket() const
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(now - lastPacketTime).count();
}