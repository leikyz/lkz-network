#ifndef LEAVE_LOBBY_MESSAGE_H
#define LEAVE_LOBBY_MESSAGE_H

#include "Protocol/Message/Message.h"

struct LeaveLobbyMessage : public Message
{
    static constexpr int ID = 7;

    LeaveLobbyMessage();

	byte positionInLobby;

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // LEAVE_LOBBY_MESSAGE_H

