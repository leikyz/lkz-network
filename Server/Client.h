#include <string>
#include <memory>

#pragma once
struct Client 
{
    int id;
    sockaddr_in address;
    std::string ipAddress;
    byte positionInLobby = -1;
    byte matchmakingMapIdRequest;
    int lobbyId = -1;
    int playerEntityId;
	bool isReady = false;
};
