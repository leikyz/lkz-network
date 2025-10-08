#ifndef MOUSE_INPUT_ENTITY_MESSAGE_H
#define MOUSE_INPUT_ENTITY_MESSAGE_H

#include "LKZ/Protocol/Message/Message.h"

struct MouseInputEntityMessage : public Message
{
    static constexpr uint8_t ID = 13;

    MouseInputEntityMessage();

    MouseInputEntityMessage(int entityId, float inputX, float inputY);

    int entityId;

    float inputY;
    float inputX;

    uint8_t getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // MOUSE_INPUT_ENTITY_MESSAGE_H
