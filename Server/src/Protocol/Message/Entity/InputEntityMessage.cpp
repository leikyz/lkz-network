#include "LKZ/Protocol/Message/Entity/InputEntityMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>

InputEntityMessage::InputEntityMessage() {};


byte InputEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& InputEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(inputX);
    serializer.writeFloat(inputY);

    return serializer.getBuffer();
}

void InputEntityMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
    inputX = deserializer.readFloat();
    inputY = deserializer.readFloat();
}


void InputEntityMessage::process(const sockaddr_in& senderAddr)
{
    auto* client = ClientManager::getClientByAddress(senderAddr);
    if (!client) return;

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby) return;

    Entity entity = entityId; // You already have entityId
    auto& components = ComponentManager::Instance();

    //// Ensure the entity exists
    if (components.positions.find(entity) != components.positions.end()) {
        components.inputs[entity] = InputComponent{ inputX, inputY };

        //    Serializer serializer;
        //    serialize(serializer);

        //    Server::SendToAllInLobbyExcept(lobby, senderAddr, serializer.buffer);
        //}
    }
}


