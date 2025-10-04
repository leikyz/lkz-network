#ifndef LAST_ENTITY_POSITION_MESSAGE_H
#define LAST_ENTITY_POSITION_MESSAGE_H

#include "Protocol/Message/Message.h"

struct LastEntityPositionMessage : public Message
{
    static constexpr int ID = 9;

    LastEntityPositionMessage();

    LastEntityPositionMessage(int entityId, float posX, float posY, float posZ);

    int entityId;

    float posX;
    float posY;
    float posZ;

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // LAST_ENTITY_POSITION_MESSAGE_H
