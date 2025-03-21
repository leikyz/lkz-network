#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"
#include "LobbyManager.h"

struct CreateEntityMessage : public Message
{
    static constexpr int ID = 4;

    CreateEntityMessage() = default;

    CreateEntityMessage(int entityId, int entityTypeId, float posX, float posY, float posZ)
        : entityId(entityId), entityTypeId(entityTypeId), posX(posX), posY(posY), posZ(posZ) {}

    int entityId;
    int entityTypeId;

    float posX;
    float posY;
    float posZ;

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override
    {
        serializer.writeInt(ID);
        serializer.writeInt(entityId);
        serializer.writeInt(entityTypeId);
        serializer.writeFloat(posX);
        serializer.writeFloat(posY);
        serializer.writeFloat(posZ);

        return serializer.buffer;
    }

    void deserialize(Deserializer& deserializer) override
    {

    }

    void process(const sockaddr_in& senderAddr) override
    {
        std::shared_ptr<Lobby> lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);
        std::shared_ptr<Entity> entity = std::make_shared<Entity>(EntityEnum::Player);
        lobby->addEntity(entity);

        entityId = entity->id;
        entityTypeId = entity->type;

        posX = 120.0f;
        posY = 100.0f;
        posZ = 100.0f;

        // Sérialisation
        Serializer serializer;
        serialize(serializer);

        // Envoyer le message de création de lobby au client
        Server::Send(senderAddr, serializer.buffer);
    }
};
