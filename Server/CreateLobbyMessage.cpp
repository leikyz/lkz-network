#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"
#include "LobbyManager.h"

struct CreateLobbyMessage : public Message
{
    static constexpr int ID = 2;

    CreateLobbyMessage() {}

    int lobbyId;

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override
    {
        serializer.writeInt(ID);
        serializer.writeInt(lobbyId);

        return serializer.buffer; 
    }

    void deserialize(Deserializer& deserializer) override
    {

    }

    void process(const sockaddr_in& senderAddr) override
    {
        // Cr�er un lobby
        LobbyManager::createLobby();

        // R�cup�rer l'ID du dernier lobby cr��
        int lastLobbyId = LobbyManager::getLastLobbyId();

        LobbyManager::addClientToLobby(lastLobbyId, ClientManager::getClientByAddress(senderAddr));

        lobbyId = lastLobbyId;

        // S�rialisation
        Serializer serializer;
        serialize(serializer);

        // Envoyer le message de cr�ation de lobby au client
        Server::Send(senderAddr, serializer.buffer);
    }
};
