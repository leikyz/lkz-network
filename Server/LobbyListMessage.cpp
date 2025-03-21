#include "Message.h"
#include "Server.h"
#include "LobbyManager.h"

struct LobbyListMessage : public Message
{
    static constexpr int ID = 5;

    LobbyListMessage() {}

    std::vector<std::pair<int, int>> lobbies; // Stocke les paires (lobbyId, nombre de clients)

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override
    {
        serializer.writeInt(ID);
        serializer.writeInt(static_cast<int>(lobbies.size())); // Écrire le nombre de lobbys

        for (const auto& lobby : lobbies)
        {
            serializer.writeInt(lobby.first);  // ID du lobby
            serializer.writeInt(lobby.second); // Nombre de clients dans le lobby
        }

        return serializer.buffer;
    }

    void deserialize(Deserializer& deserializer) override
    {

    }

    void process(const sockaddr_in& senderAddr) override
    {
        lobbies.clear();
        const auto& allLobbies = LobbyManager::getAllLobbies(); // Supposons que cette méthode existe

        for (const auto& lobby : allLobbies)
        {
            int id = lobby->id;
            int clients = lobby->clients.size();
            lobbies.emplace_back(id, clients);
        }

        // Sérialisation
        Serializer serializer;
        serialize(serializer);

        // Envoyer la liste des lobbys au client
        Server::Send(senderAddr, serializer.buffer);
    }
};

