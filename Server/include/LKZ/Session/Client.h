#include <string>
#include <memory>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#pragma once
struct Client
{
    uint32_t id;
    int lobbyId;
    int playerEntityId;
    sockaddr_in address;
    std::string ipAddress;
    uint8_t positionInLobby;
    uint8_t matchmakingMapIdRequest;
    bool isReady;

    Client(uint32_t _id, const sockaddr_in& _address, const std::string& _ipAddress, int _lobbyId = -1, int _playerEntityId = -1
        , uint8_t _positionInLobby = 0, uint8_t _matchmakingMapIdRequest = 0, bool _isReady = false) : id(_id),
        lobbyId(_lobbyId),
        playerEntityId(_playerEntityId),
        address(_address),
        ipAddress(_ipAddress),
        positionInLobby(_positionInLobby),
        matchmakingMapIdRequest(_matchmakingMapIdRequest),
        isReady(_isReady)
    {
    }
};