#include "LKZ/Protocol/Message/Entity/MouseInputEntityMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>

MouseInputEntityMessage::MouseInputEntityMessage() {};


uint8_t MouseInputEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& MouseInputEntityMessage::serialize(Serializer& serializer) const
{
    return serializer.getBuffer();
}

void MouseInputEntityMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
	inputX = deserializer.readFloat();
    inputY = deserializer.readFloat();
}


void MouseInputEntityMessage::process(const sockaddr_in& senderAddr)
{
    auto* client = ClientManager::getClientByAddress(senderAddr);
    if (!client) return;

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby) return;

    Entity entity = entityId; // You already have entityId
    auto& components = ComponentManager::Instance();

    //// Ensure the entity exists
    if (components.positions.find(entity) != components.positions.end())
        components.mouseInputs[entity] = MouseInputComponent{ inputX, inputY };

        //    Serializer serializer;
        //    serialize(serializer);

        //    Server::SendToAllInLobbyExcept(lobby, senderAddr, serializer.buffer);
        //}*/

}



