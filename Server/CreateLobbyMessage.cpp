#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"
#include "LobbyManager.h"

struct CreateLobbyMessage : public Message
{
    static constexpr int ID = 2;

    CreateLobbyMessage() {}

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(std::vector<uint8_t>& buffer) const override
    {
        buffer.clear();

        buffer.push_back(static_cast<uint8_t>(ID & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 24) & 0xFF));


        return buffer;
    }

    void deserialize(const std::vector<uint8_t>& buffer) override
    {

    }

    void process(const sockaddr_in& senderAddr) const override
    {
     
        LobbyManager::createLobby(1);
        LobbyManager::addClientToLobby(1, ClientManager::getClientByAddress(senderAddr));

        CreateLobbyMessage message;
        Server::Send(senderAddr, message);

    }
};
