#include "LKZ/Protocol/Message/Entity/RotateEntityMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>

RotateEntityMessage::RotateEntityMessage() {};


byte RotateEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& RotateEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(rotaY);

    return serializer.getBuffer();
}

void RotateEntityMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
    rotaY = deserializer.readFloat();
}


void RotateEntityMessage::process(const sockaddr_in& senderAddr)
{
    auto* client = ClientManager::getClientByAddress(senderAddr);
    if (!client) return;

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby) return;

    Entity entity = entityId; // You already have entityId
    auto& components = ComponentManager::Instance();

    //// Ensure the entity exists
   /* if (components.positions.find(entity) != components.positions.end()) {
        components.inputs[entity] = InputComponent{ inputX, inputY };*/

        //    Serializer serializer;
        //    serialize(serializer);

        //    Server::SendToAllInLobbyExcept(lobby, senderAddr, serializer.buffer);
        //}
}

