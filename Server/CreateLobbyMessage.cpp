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
        // Créer un lobby
        LobbyManager::createLobby();

        // Récupérer l'ID du dernier lobby créé
        int lastLobbyId = LobbyManager::getLastLobbyId();

        LobbyManager::addClientToLobby(lastLobbyId, ClientManager::getClientByAddress(senderAddr));

        lobbyId = lastLobbyId;

        // Sérialisation
        Serializer serializer;
        serialize(serializer);

        // Envoyer le message de création de lobby au client
        Server::Send(senderAddr, serializer.buffer);
    }
};
