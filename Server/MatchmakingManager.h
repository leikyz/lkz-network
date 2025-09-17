#pragma once
#include <vector>
#include <WinSock2.h>
#include "Client.h"

class MatchmakingManager
{
public:
    static void AddPlayerToQueue(Client& playerAddr);
    static void Update(); 
    static void RemovePlayerFromQueue(const sockaddr_in& addr);

private:
    static std::vector<Client> waitingPlayers;

    static void StartMatch(const std::vector<sockaddr_in>& players);
};
