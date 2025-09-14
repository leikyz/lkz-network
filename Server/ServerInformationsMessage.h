#ifndef SERVER_INFORMATIONS_MESSAGES_H
#define SERVER_INFORMATIONS_MESSAGES_H

#include "Message.h"

struct ServerInformationsMessage : public Message
{
    static constexpr int ID = 2;

    ServerInformationsMessage();

    bool status;
    int playersCount;

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // SERVER_INFORMATIONS_MESSAGES_H
