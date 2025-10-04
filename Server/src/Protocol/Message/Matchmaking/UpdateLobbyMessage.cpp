#include "Protocol/Message/Matchmaking/UpdateLobbyMessage.h"

UpdateLobbyMessage::UpdateLobbyMessage()  
{
}

std::vector<uint8_t>& UpdateLobbyMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeByte(updatedLobbyPos);
    serializer.writeByte(playersCount);

    for (byte index : playersInLobby)
    {
        serializer.writeByte(index);
    }

    return serializer.getBuffer();
}

void UpdateLobbyMessage::deserialize(Deserializer& deserializer)
{
}

void UpdateLobbyMessage::process(const sockaddr_in& senderAddr)
{
}
