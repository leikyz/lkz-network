#pragma once
#include <vector>
#include <WinSock2.h>

class MatchmakingManager
{
public:
    static void AddPlayerToQueue(const sockaddr_in& playerAddr);
    static void Update(); 

private:
    static std::vector<sockaddr_in> waitingPlayers;

    static void StartMatch(const std::vector<sockaddr_in>& players);
};
