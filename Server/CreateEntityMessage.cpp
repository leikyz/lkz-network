#include "CreateEntityMessage.h"
#include <cstdlib> 
#include <ctime>   

CreateEntityMessage::CreateEntityMessage() {};

CreateEntityMessage::CreateEntityMessage(int entityId, int entityTypeId, float posX, float posY, float posZ)
    : entityId(entityId), entityTypeId(entityTypeId), posX(posX), posY(posY), posZ(posZ)
{
}

int CreateEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& CreateEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeInt(ID);
    serializer.writeInt(entityId);
    serializer.writeInt(entityTypeId);
    serializer.writeFloat(posX);
    serializer.writeFloat(posY);
    serializer.writeFloat(posZ);

    return serializer.buffer;
}

void CreateEntityMessage::deserialize(Deserializer& deserializer)
{
}


void CreateEntityMessage::process(const sockaddr_in& senderAddr)
{
    std::shared_ptr<Lobby> lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);
    
    if (lobby != nullptr)
    {
        srand(time(0));
        std::shared_ptr<Entity> entity;

        if (lobby->clients.size() == 1)
        {
            entity = std::make_shared<Entity>(EntityEnum::Olise);
        }
        else
        {
            entity = std::make_shared<Entity>(EntityEnum::Elisa);
        }

        entity->posX = 100.0f + rand() % 10;
        entity->posY = 20;
        entity->posZ = 100.0f + rand() % 10;

        lobby->addEntity(entity);

        entityId = entity->id;
        entityTypeId = entity->type;
        posX = entity->posX;
        posY = entity->posY;
        posZ = entity->posZ;

        ClientManager::getClientByAddress(senderAddr)->playerEntityId = entityId;

        Serializer serializer;
        serialize(serializer);

        Server::SendToAllInLobby(lobby, serializer.buffer);
    }
}

