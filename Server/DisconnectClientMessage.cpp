#include "DisconnectClientMessage.h"

DisconnectClientMessage::DisconnectClientMessage() {}

byte DisconnectClientMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& DisconnectClientMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    return serializer.buffer;
}

void DisconnectClientMessage::deserialize(Deserializer& deserializer)
{

}

void DisconnectClientMessage::process(const sockaddr_in& senderAddr)
{
    ClientManager::removeClient(senderAddr);
}
