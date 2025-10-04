#ifndef LOBBY_MANAGER_H
#define LOBBY_MANAGER_H

#include <unordered_map>
#include <vector>
#include <atomic>
#include <mutex>
#include <string>
#include "LKZ/NetworkObject/Lobby.h"

class LobbyManager
{
public:
    static void createLobby(byte mapId);
    static void addClientToLobby(int lobbyId, Client* client);
    static Lobby* getLobby(int lobbyId);
    static void removeLobby(int lobbyId);

    static int getLastLobbyId()
    {
        return nextLobbyId.load() - 1;
    }

    static int getLobbyCount();

    static Lobby* getAvailableLobby(byte mapId);

    static std::vector<Lobby*> getAllLobbies();

    static bool IsEveryoneReadyInLobby(int lobbyId);

private:
    static std::unordered_map<int, Lobby*> lobbies;
    static std::atomic<int> nextLobbyId;
    static std::mutex lobbiesMutex; 
};

#endif // LOBBY_MANAGER_H
