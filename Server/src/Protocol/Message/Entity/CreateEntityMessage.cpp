#include "LKZ/Protocol/Message/Entity/CreateEntityMessage.h"
#include <cstdlib> 
#include <ctime>   

CreateEntityMessage::CreateEntityMessage() {};

CreateEntityMessage::CreateEntityMessage(int entityId, int entityTypeId, float posX, float posY, float posZ)
    : entityId(entityId), entityTypeId(entityTypeId), posX(posX), posY(posY), posZ(posZ)
{
}

byte CreateEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& CreateEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeInt(entityId);
    serializer.writeInt(entityTypeId);
    serializer.writeFloat(posX);
    serializer.writeFloat(posY);
    serializer.writeFloat(posZ);
    serializer.writeBool(controlled);

    return serializer.getBuffer();
}

void CreateEntityMessage::deserialize(Deserializer& deserializer)
{
}


void CreateEntityMessage::process(const sockaddr_in& senderAddr)
{
    Lobby* lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);
    
    if (lobby != nullptr)
    {
        srand(time(0));
        Entity* entity;

        if (lobby->clients.size() == 1)
        {
            entity = new Entity(EntityEnum::Olise);
        }
        else
        {
            entity = new Entity(EntityEnum::Olise);
        }

        entity->posX = 100.0f + rand() % 10;
        entity->posY = 10;
        entity->posZ = 100.0f + rand() % 10;

        lobby->addEntity(entity);

        entityId = entity->id;
        entityTypeId = entity->type;
        posX = entity->posX;
        posY = entity->posY;
        posZ = entity->posZ;

        ClientManager::getClientByAddress(senderAddr)->playerEntityId = entityId;

        controlled = (ClientManager::getClientByAddress(senderAddr)->playerEntityId == entityId);

     

        Serializer serializer;
        serialize(serializer);

        Server::Send(senderAddr, serializer.getBuffer());

        controlled = false;

        /*Serializer serializer;*/
		serialize(serializer);

        Server::SendToAllInLobbyExcept(lobby, senderAddr, serializer.getBuffer());
    }
}

