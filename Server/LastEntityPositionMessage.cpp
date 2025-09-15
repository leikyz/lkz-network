#include "LastEntityPositionMessage.h"
#include <cstdlib> 
#include <ctime>   

LastEntityPositionMessage::LastEntityPositionMessage() {};

LastEntityPositionMessage::LastEntityPositionMessage(int entityId, float posX, float posY, float posZ)
    : entityId(entityId), posX(posX), posY(posY), posZ(posZ)
{
}

byte LastEntityPositionMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& LastEntityPositionMessage::serialize(Serializer& serializer) const
{
    serializer.writeInt(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(posX);
    serializer.writeFloat(posY);
    serializer.writeFloat(posZ);

    return serializer.buffer;
}

void LastEntityPositionMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
    posX = deserializer.readFloat();
    posY = deserializer.readFloat();
    posZ = deserializer.readFloat();
}


void LastEntityPositionMessage::process(const sockaddr_in& senderAddr)
{
    Lobby* lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);
    Entity* entity = lobby->getEntityById(entityId);

    if (lobby != nullptr && entity != nullptr)
    {
        entity->posX = posX;
        entity->posY = posY;
        entity->posZ = posZ;

        Serializer serializer;
        serialize(serializer);

        Server::SendToAllInLobbyExcept(lobby, senderAddr, serializer.buffer);
    }
  
}

