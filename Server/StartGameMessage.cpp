#include "StartGameMessage.h"

StartGameMessage::StartGameMessage() {}

byte StartGameMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& StartGameMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
	serializer.writeByte(mapId);
    return serializer.buffer;
}

void StartGameMessage::deserialize(Deserializer& deserializer)
{
}

void StartGameMessage::process(const sockaddr_in& senderAddr)
{
}
