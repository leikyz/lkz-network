#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"
#include "LobbyManager.h"

struct CreateEntityMessage : public Message
{
    static constexpr int ID = 4;

    CreateEntityMessage() {}

    int lobbyId;
    int entityId;
    int entityTypeId;

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override
    {
        serializer.writeInt(ID);
        serializer.writeInt(entityId);
        serializer.writeInt(entityTypeId);

        return serializer.buffer;
    }

    void deserialize(Deserializer& deserializer) override
    {
        lobbyId = deserializer.readInt();
    }

    void process(const sockaddr_in& senderAddr) override
    {
        std::shared_ptr<Lobby> lobby = LobbyManager::getLobby(lobbyId);
        std::shared_ptr<Entity> entity = std::make_shared<Entity>(EntityEnum::Player);
        lobby->addEntity(entity);

        entityId = entity->id;
        entityTypeId = entity->type;

        // Sérialisation
        Serializer serializer;
        serialize(serializer);

        // Envoyer le message de création de lobby au client
        Server::Send(senderAddr, serializer.buffer);
    }
};
