#ifndef DISCONNECT_CLIENT_MESSAGE_H
#define DISCONNECT_CLIENT_MESSAGE_H

#include "Message.h"

struct DisconnectClientMessage : public Message
{
    static constexpr int ID = 1;

    DisconnectClientMessage();

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // DISCONNECT_CLIENT_MESSAGE_H
