#ifndef ROTATE_ENTITY_MESSAGE_H
#define ROTATE_ENTITY_MESSAGE_H

#include "LKZ/Protocol/Message/Message.h"

struct RotateEntityMessage : public Message
{
    static constexpr uint8_t ID = 12;

    RotateEntityMessage();

    RotateEntityMessage(int entityId, float rotaY);

    int entityId;

    float rotaY;

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // ROTATE_ENTITY_MESSAGE_H
