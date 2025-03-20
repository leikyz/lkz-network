#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"
#include "LobbyManager.h"

struct CreateLobbyMessage : public Message
{
    static constexpr int ID = 2;

    CreateLobbyMessage() {}

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override
    {
        serializer.writeInt(ID);
        return serializer.buffer; // Retourner le buffer de serializer
    }

    void deserialize(Deserializer& deserializer) override
    {
        int receivedId = deserializer.readInt();
        if (receivedId != ID) {
            throw std::runtime_error("Mauvais ID de message reçu !");
        }
    }

    void process(const sockaddr_in& senderAddr) const override
    {
        LobbyManager::createLobby(1);
        LobbyManager::addClientToLobby(1, ClientManager::getClientByAddress(senderAddr));

        Serializer serializer;
        serialize(serializer);     
    }
};
