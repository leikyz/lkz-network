#ifndef LOBBY_LIST_MESSAGE_H
#define LOBBY_LIST_MESSAGE_H

#include "Message.h"
#include <vector>
#include <utility>

struct LobbyListMessage : public Message
{
    static constexpr int ID = 5;

    LobbyListMessage();

    std::vector<std::pair<int, int>> lobbies; // lobby id and clients count

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // LOBBY_LIST_MESSAGE_H
