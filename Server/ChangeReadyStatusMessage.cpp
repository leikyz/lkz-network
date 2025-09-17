#include "ChangeReadyStatusMessage.h"
#include "StartGameMessage.h"
ChangeReadyStatusMessage::ChangeReadyStatusMessage() {}

byte ChangeReadyStatusMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& ChangeReadyStatusMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeBool(isReady);
	serializer.writeByte(positionInLobby);
    return serializer.buffer;
}


void ChangeReadyStatusMessage::deserialize(Deserializer& deserializer)
{
	isReady = deserializer.readBool();
}

void ChangeReadyStatusMessage::process(const sockaddr_in& senderAddr)
{
	Client* currentClient = ClientManager::getClientByAddress(senderAddr);

	Lobby* lobby = LobbyManager::getLobby(currentClient->lobbyId);

    if (currentClient && lobby)
    {
        currentClient->isReady = isReady;
		positionInLobby = currentClient->positionInLobby;

        Serializer serializer;
        serialize(serializer);
        Server::SendToAllInLobby(lobby, serializer.buffer);

        if (LobbyManager::IsEveryoneReadyInLobby(lobby->id))
        {
            StartGameMessage startGameMsg;
			startGameMsg.mapId = lobby->mapId;
            Serializer s;
            std::vector<uint8_t> buf = startGameMsg.serialize(s);

			Server::SendToAllInLobby(lobby, buf);
        }
	}


}
