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
    return serializer.getBuffer();
}

void StartMatchmakingMessage::deserialize(Deserializer& deserializer)
{
    mapId = deserializer.readByte();
}

void StartMatchmakingMessage::process(const sockaddr_in& senderAddr)
{
    Serializer serializer;
    serialize(serializer);

	Client* client = ClientManager::getClientByAddress(senderAddr);
	client->m_matchmakingMapIdRequest = mapId;
	MatchmakingManager::AddPlayerToQueue(*client);

    Server::Send(senderAddr, serializer.getBuffer());
}
