#include "LKZ/Core/Server/WindowsServer.h"
#include <iostream>
#include <LKZ/Manager/EventManager.h>

WindowsServer::WindowsServer(int port, size_t bufferSize)
    : port(port), bufferSize(bufferSize) {}

WindowsServer::~WindowsServer() {
    running = false;
    if (completionPort) CloseHandle(completionPort);
    if (listenSocket != INVALID_SOCKET) closesocket(listenSocket);
    WSACleanup();
}

void WindowsServer::Start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[WindowsServer] WSAStartup failed\n";
        return;
    }

    listenSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, WSA_FLAG_OVERLAPPED);

    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "[WindowsServer] socket creation failed\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[WindowsServer] bind failed\n";
        return;
    }

    InitIOCP();
	running = true;

    std::cout << "[WindowsServer] Server started on port " << port << "\n";
}

void WindowsServer::InitIOCP() {
    completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (!completionPort) {
        std::cerr << "[WindowsServer] CreateIoCompletionPort failed\n";
        return;
    }

    if (!CreateIoCompletionPort((HANDLE)listenSocket, completionPort, 0, 0)) {
        std::cerr << "[WindowsServer] Associate socket with IOCP failed\n";
        return;
    }

    // Preallocate IoData
    for (size_t i = 0; i < std::thread::hardware_concurrency() * 2; i++) {
        auto ioData = std::make_unique<IoData>(bufferSize);
        PostReceive(ioData.get());
        ioDataPool.push_back(std::move(ioData));
    }
}

void WindowsServer::PostReceive(IoData* ioData) {
    DWORD flags = 0;
    ioData->clientAddrLen = sizeof(ioData->clientAddr);
    ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));

    int ret = WSARecvFrom(listenSocket, &ioData->wsabuf, 1, nullptr, &flags,
                           (sockaddr*)&ioData->clientAddr, &ioData->clientAddrLen,
                           &ioData->overlapped, nullptr);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[WindowsServer] WSARecvFrom failed\n";
    }
}

void WindowsServer::NotifyThreadPool(IoData* ioData, DWORD bytesTransferred) {
    auto task = [this, ioData, bytesTransferred]() {
        if (bytesTransferred > 0) {
            // Process received packet
            std::cout << "[WindowsServer] Received " << bytesTransferred << " bytes\n";

            std::vector<uint8_t> receivedData(ioData->buffer.begin(),
                ioData->buffer.begin() + bytesTransferred);

            int id = static_cast<int>(receivedData[0]);

            EventManager::processMessage(receivedData, ioData->clientAddr);
        }

        // Repost receive
        PostReceive(ioData);
        };

    ThreadManager::EnqueueTask("network", task);
}


void WindowsServer::Poll() {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    OVERLAPPED* overlapped;

    while (running) {
;
        BOOL success = GetQueuedCompletionStatus(completionPort, &bytesTransferred, &completionKey, &overlapped, INFINITE);
      
        if (!overlapped) continue;

        IoData* ioData = CONTAINING_RECORD(overlapped, IoData, overlapped);

        NotifyThreadPool(ioData, bytesTransferred);
    }
}

void WindowsServer::Send(const sockaddr_in& clientAddr, const std::vector<uint8_t>& buffer) {
    WSABUF sendBuf{};
    sendBuf.buf = (CHAR*)buffer.data();
    sendBuf.len = static_cast<ULONG>(buffer.size());

    DWORD bytesSent;
    WSASendTo(listenSocket, &sendBuf, 1, &bytesSent, 0,
              (sockaddr*)&clientAddr, sizeof(clientAddr), nullptr, nullptr);
}
