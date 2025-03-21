#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"
#include "LobbyManager.h"
#include "CreateEntityMessage.cpp"

struct SynchronizeEntitiesMessage : public Message
{
    static constexpr int ID = 6;

    SynchronizeEntitiesMessage() {}

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override
    {
        serializer.writeInt(ID);
        return serializer.buffer;
    }

    void deserialize(Deserializer& deserializer) override
    {

    }

    void process(const sockaddr_in& senderAddr) override
    {
        std::shared_ptr<Lobby> lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);

        if (!lobby)
        {
            std::cerr << "Lobby introuvable pour le client." << std::endl;
            return;
        }

        for (const auto& entity : lobby->entities)
        {
            if (entity)
            {
                // Initialisation correcte
                CreateEntityMessage createEntityMessage(entity->id, entity->type, entity->posX, entity->posY, entity->posZ);

                Serializer serializer;
                createEntityMessage.serialize(serializer);
                Server::Send(senderAddr, serializer.buffer);
            }
        }
    }

};
