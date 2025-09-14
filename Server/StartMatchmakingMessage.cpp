#include "StartMatchmakingMessage.h"
#include "MatchmakingManager.h"

StartMatchmakingMessage::StartMatchmakingMessage() {}

byte StartMatchmakingMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& StartMatchmakingMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    return serializer.buffer;
}

void StartMatchmakingMessage::deserialize(Deserializer& deserializer)
{

}

void StartMatchmakingMessage::process(const sockaddr_in& senderAddr)
{
    Serializer serializer;
    serialize(serializer);

	MatchmakingManager::AddPlayerToQueue(senderAddr);

    Server::Send(senderAddr, serializer.buffer);
}
