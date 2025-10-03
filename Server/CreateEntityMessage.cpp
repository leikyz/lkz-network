#include "CreateEntityMessage.h"
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
    Lobby* lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->m_lobbyId);
    
    if (lobby != nullptr)
    {
        srand(time(0));
        Entity* entity;

        if (lobby->m_clients.size() == 1)
        {
            entity = new Entity(EntityEnum::Olise);
        }
        else
        {
            entity = new Entity(EntityEnum::Olise);
        }

        entity->m_posX = 100.0f + rand() % 10;
        entity->m_posY = 10;
        entity->m_posZ = 100.0f + rand() % 10;

        lobby->addEntity(entity);

        entityId = entity->m_id;
        entityTypeId = entity->m_type;
        posX = entity->m_posX;
        posY = entity->m_posY;
        posZ = entity->m_posZ;

        ClientManager::getClientByAddress(senderAddr)->m_playerEntityId = entityId;

        controlled = (ClientManager::getClientByAddress(senderAddr)->m_playerEntityId == entityId);

     

        Serializer serializer;
        serialize(serializer);

        Server::Send(senderAddr, serializer.getBuffer());

        controlled = false;

        /*Serializer serializer;*/
		serialize(serializer);

        Server::SendToAllInLobbyExcept(lobby, senderAddr, serializer.getBuffer());
    }
}

