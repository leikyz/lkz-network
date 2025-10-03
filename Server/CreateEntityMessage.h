#ifndef CREATE_ENTITY_MESSAGE_H
#define CREATE_ENTITY_MESSAGE_H

#include "Message.h"

struct CreateEntityMessage : public Message
{
    static constexpr int ID = 9;

    CreateEntityMessage();

    CreateEntityMessage(int entityId, int entityTypeId, float posX, float posY, float posZ);

    int entityId;
    int entityTypeId;

    float posX;
    float posY;
    float posZ;

    bool controlled;

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // CREATE_ENTITY_MESSAGE_H
