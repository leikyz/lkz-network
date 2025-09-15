#include "JoinLobbyMessage.h"

JoinLobbyMessage::JoinLobbyMessage() {}

byte JoinLobbyMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& JoinLobbyMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
	serializer.writeByte(positionInLobby);
    return serializer.buffer;
}


void JoinLobbyMessage::deserialize(Deserializer& deserializer)
{
}

void JoinLobbyMessage::process(const sockaddr_in& senderAddr)
{
}
