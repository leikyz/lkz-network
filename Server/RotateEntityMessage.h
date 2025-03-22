#ifndef ROTATE_ENTITY_MESSAGE_H
#define ROTATE_ENTITY_MESSAGE_H

#include "Message.h"

struct RotateEntityMessage : public Message
{
    static constexpr int ID = 8;

    RotateEntityMessage();

    RotateEntityMessage(int entityId, float mouseInputX, float mouseInputY);

    int entityId;

    float mouseInputX;
    float mouseInputY;

    int getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // ROTATE_ENTITY_MESSAGE_H
