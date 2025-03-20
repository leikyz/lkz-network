#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"
#include "LobbyManager.h"

struct JoinLobbyMessage : public Message
{
    static constexpr int ID = 3;

    JoinLobbyMessage() {}

    int lobbyId;

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override
    {
        serializer.writeInt(ID);
        serializer.writeInt(lobbyId);

        return serializer.buffer;
    }

    void deserialize(Deserializer& deserializer) override
    {
        lobbyId = deserializer.readInt();
    }

    void process(const sockaddr_in& senderAddr) override
    {
        std::shared_ptr<Lobby> lobby = LobbyManager::getLobby(lobbyId);
        // Sérialisation
        Serializer serializer;
        serialize(serializer);

        // Envoyer le message de création de lobby au client
        Server::Send(senderAddr, serializer.buffer);
    }
};
