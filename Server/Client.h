#include <string>
#include <memory>

#pragma once
struct Client 
{
    int id;
    sockaddr_in address;
    std::string ipAddress;
    int lobbyId;
    int playerEntityId;
	byte matchmakingMapIdRequest;
  //  std::shared_ptr<Lobby> lobby;
};
