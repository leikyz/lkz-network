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
    return serializer.getBuffer();
}


void ChangeReadyStatusMessage::deserialize(Deserializer& deserializer)
{
	isReady = deserializer.readBool();
}

void ChangeReadyStatusMessage::process(const sockaddr_in& senderAddr)
{
	Client* currentClient = ClientManager::getClientByAddress(senderAddr);

	Lobby* lobby = LobbyManager::getLobby(currentClient->m_lobbyId);

    if (currentClient && lobby)
    {
        currentClient->m_isReady = isReady;
		positionInLobby = currentClient->m_positionInLobby;

        Serializer serializer;
        serialize(serializer);
        Server::SendToAllInLobby(lobby, serializer.getBuffer());

        if (LobbyManager::IsEveryoneReadyInLobby(lobby->m_id))
        {
            StartGameMessage startGameMsg;
			startGameMsg.mapId = lobby->m_mapId;
            Serializer s;
            std::vector<uint8_t> buf = startGameMsg.serialize(s);

			Server::SendToAllInLobby(lobby, buf);
        }
	}


}
