#include "LKZ/Core/Server/ProfilerServer.h"
#include <iostream>
#include <string>
#include <Common/ProfilerProtocol.h>

ProfilerServer::ProfilerServer(int port)
    : port(port) {
}

ProfilerServer::~ProfilerServer()
{
    Stop();
}

void ProfilerServer::Start()
{
    // On suppose que WSAStartup est déjà fait par le serveur principal ou Engine
    // Si ce n'est pas le cas, décommente les lignes suivantes :
    /*
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[Profiler] WSAStartup failed\n";
        return;
    }
    */

    listenSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "[Profiler] Socket creation failed\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // DEBUG : Vérification du Bind
    /*std::cout << "[Profiler] Trying to bind port " << port << "...\n";*/

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[Profiler] Bind failed! Error: " << WSAGetLastError() << "\n";
        return;
    }

    InitIOCP();
    running = true;
  /*  std::cout << "[Profiler] Started on port " << port << " (Waiting for handshake...)\n";*/
}

void ProfilerServer::Stop()
{
    running = false;
    if (completionPort) CloseHandle(completionPort);
    if (listenSocket != INVALID_SOCKET) closesocket(listenSocket);
}

void ProfilerServer::InitIOCP()
{
    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (!completionPort) {
        std::cerr << "[Profiler] CreateIoCompletionPort failed (1)\n";
        return;
    }

    if (!CreateIoCompletionPort((HANDLE)listenSocket, completionPort, 0, 0)) {
        std::cerr << "[Profiler] CreateIoCompletionPort failed (2)\n";
        return;
    }

    // On prépare quelques buffers pour recevoir les Handshakes
    for (size_t i = 0; i < 4; i++)
    {
        auto ioData = std::make_unique<ProfilerIoData>(1024);

        // On récupère le pointeur brut pour l'appel avant de déplacer le unique_ptr dans le vecteur
        ProfilerIoData* ptr = ioData.get();
        ioDataPool.push_back(std::move(ioData));

        PostReceive(ptr);
    }
}

void ProfilerServer::PostReceive(ProfilerIoData* ioData)
{
    DWORD flags = 0;
    ioData->clientAddrLen = sizeof(ioData->clientAddr);
    ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));

    int ret = WSARecvFrom(listenSocket, &ioData->wsabuf, 1, nullptr, &flags,
        (sockaddr*)&ioData->clientAddr, &ioData->clientAddrLen,
        &ioData->overlapped, nullptr);

    // DEBUG : Vérification immédiate de l'erreur
    if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            std::cerr << "[Profiler] WSARecvFrom failed! Error: " << err << "\n";
        }
    }
}

void ProfilerServer::Poll()
{
    if (!running) return;

    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    OVERLAPPED* overlapped;

    // Timeout à 0 pour ne pas bloquer le thread
    BOOL success = GetQueuedCompletionStatus(
        completionPort,
        &bytesTransferred,
        &completionKey,
        &overlapped,
        0
    );

    if (success && overlapped)
    {
        ProfilerIoData* ioData = CONTAINING_RECORD(overlapped, ProfilerIoData, overlapped);
        HandlePacket(ioData, bytesTransferred);
        PostReceive(ioData); // On remet en écoute immédiatement
    }
    // Gérer le cas où GetQueuedCompletionStatus échoue mais retourne un overlapped (déconnexion abrupte par ex)
    else if (!success && overlapped)
    {
        ProfilerIoData* ioData = CONTAINING_RECORD(overlapped, ProfilerIoData, overlapped);
        PostReceive(ioData);
    }
}

void ProfilerServer::HandlePacket(ProfilerIoData* ioData, DWORD bytesTransferred)
{
    if (bytesTransferred == 0) return;

    // DEBUG : Afficher qu'on a reçu quelque chose
    // std::cout << "[Profiler] Received " << bytesTransferred << " bytes\n";

    uint8_t packetID = ioData->buffer[0];

    // Si on reçoit un Handshake (255), on ajoute le client à la liste
    if (packetID == ProfilerProtocol::PacketID::Handshake)
    {
        std::lock_guard<std::mutex> lock(subscribersMutex);

        bool known = false;
        for (const auto& sub : subscribers) {
            if (sub.sin_addr.s_addr == ioData->clientAddr.sin_addr.s_addr &&
                sub.sin_port == ioData->clientAddr.sin_port) {
                known = true;
                break;
            }
        }

        if (!known) {
            subscribers.push_back(ioData->clientAddr);

            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(ioData->clientAddr.sin_addr), ip, INET_ADDRSTRLEN);
            /*std::cout << "[Profiler] New tool connected: " << ip << ":" << ntohs(ioData->clientAddr.sin_port) << "\n";*/
        }
    }
}

void ProfilerServer::Broadcast(const std::vector<uint8_t>& data)
{
    std::lock_guard<std::mutex> lock(subscribersMutex);
    for (const auto& addr : subscribers)
    {
        // DEBUG : Voir à qui on envoie
        // std::cout << "[Profiler] Sending " << data.size() << " bytes to subscriber...\n";
        SendInternal(addr, data);
    }
}

void ProfilerServer::SendInternal(const sockaddr_in& target, const std::vector<uint8_t>& buffer)
{
    WSABUF sendBuf{};
    sendBuf.buf = (CHAR*)buffer.data();
    sendBuf.len = static_cast<ULONG>(buffer.size());
    DWORD bytesSent;

    // Envoi standard (non-bloquant par nature en UDP tant que le buffer OS n'est pas plein)
    if (WSASendTo(listenSocket, &sendBuf, 1, &bytesSent, 0, (sockaddr*)&target, sizeof(target), nullptr, nullptr) == SOCKET_ERROR)
    {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            std::cerr << "[Profiler] Send failed: " << err << "\n";
        }
    }
}