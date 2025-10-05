#include "LKZ/Protocol/Message/Matchmaking/ChangeReadyStatusMessage.h"
#include "LKZ/Protocol/Message/Matchmaking/StartGameMessage.h"
ChangeReadyStatusMessage::ChangeReadyStatusMessage() {}

uint8_t ChangeReadyStatusMessage::getId() const
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

	Lobby* lobby = LobbyManager::getLobby(currentClient->lobbyId);

    if (currentClient && lobby)
    {
        currentClient->isReady = isReady;
		positionInLobby = currentClient->positionInLobby;

        Serializer serializer;
        serialize(serializer);
      /*  Server::SendToAllInLobby(lobby, serializer.getBuffer());*/

        if (LobbyManager::IsEveryoneReadyInLobby(lobby->id))
        {
            StartGameMessage startGameMsg;
			startGameMsg.mapId = lobby->mapId;
            Serializer s;
            std::vector<uint8_t> buf = startGameMsg.serialize(s);

			/*Server::SendToAllInLobby(lobby, buf);*/

            Engine::Instance().Server()->SendToMultiple(lobby->clients, serializer.getBuffer(), getClassName());
        }
	}


}
