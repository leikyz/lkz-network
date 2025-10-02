#include <string>
#include <memory>

#pragma once
struct Client 
{
    int m_id;
    sockaddr_in m_address;
    std::string m_ipAddress;
    byte m_positionInLobby = -1;
    byte m_matchmakingMapIdRequest;
    int m_lobbyId = -1;
    int m_playerEntityId;
	bool m_isReady = false;
	std::string m_name = "Anonymous";
};
