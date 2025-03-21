#ifndef CREATE_LOBBY_MESSAGE_H
#define CREATE_LOBBY_MESSAGE_H

#include "Message.h"

struct CreateLobbyMessage : public Message
{
    static constexpr int ID = 2;

    CreateLobbyMessage();

    int lobbyId;

    int getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // CREATE_LOBBY_MESSAGE_H
