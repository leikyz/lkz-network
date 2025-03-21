#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"
#include "LobbyManager.h"

struct CreateLobbyMessage : public Message
{
    static constexpr int ID = 2;

    CreateLobbyMessage() {}

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

    }

    void process(const sockaddr_in& senderAddr) override
    {
        LobbyManager::createLobby();

        int lastLobbyId = LobbyManager::getLastLobbyId();

        lobbyId = lastLobbyId;

        // Sérialisation
        Serializer serializer;
        serialize(serializer);

        Server::Send(senderAddr, serializer.buffer);
    }
};
