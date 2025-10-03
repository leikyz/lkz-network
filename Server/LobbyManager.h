#ifndef LOBBY_MANAGER_H
#define LOBBY_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include "Lobby.h"

class LobbyManager
{
public:
    static void createLobby(byte mapId);
    static void addClientToLobby(int lobbyId, Client* client);
    static Lobby* getLobby(int lobbyId);
    static void removeLobby(int lobbyId);
    static int nextLobbyId;

    static int getLastLobbyId()
    {
        return nextLobbyId - 1;
    }

    static int getLobbyCount() 
    {
        return static_cast<int>(lobbies.size());
    }
    static Lobby* getAvailableLobby(byte mapId);

    static std::vector<Lobby*> getAllLobbies();

    static bool IsEveryoneReadyInLobby(int lobbyId)
    {
        Lobby* lobby = getLobby(lobbyId);
        if (!lobby) return false;
        for (Client* c : lobby->m_clients)
        {
            if (!c || !c->m_isReady)
                return false;
        }
        return true;
	}


private:
    static std::unordered_map<int, Lobby*> lobbies;
};

#endif // LOBBY_MANAGER_H
