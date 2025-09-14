#ifndef LOBBY_MANAGER_H
#define LOBBY_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include "Lobby.h"

class LobbyManager
{
public:
    static void createLobby();
    static void addClientToLobby(int lobbyId, const std::shared_ptr<Client>& client);
    static std::shared_ptr<Lobby> getLobby(int lobbyId);
    static int nextLobbyId;

    static int getLastLobbyId()
    {
        return nextLobbyId - 1;
    }

    static int getLobbyCount() 
    {
        return static_cast<int>(lobbies.size());
    }
    static std::shared_ptr<Lobby> getAvailableLobby();

    static std::vector<std::shared_ptr<Lobby>> getAllLobbies();


private:
    static std::unordered_map<int, std::shared_ptr<Lobby>> lobbies;
};

#endif // LOBBY_MANAGER_H
