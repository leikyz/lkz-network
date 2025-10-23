#ifndef INPUT_ENTITY_MESSAGE_H
#define INPUT_ENTITY_MESSAGE_H

#include "LKZ/Protocol/Message/Message.h"

struct InputEntityMessage : public Message
{
    static constexpr uint8_t ID = 10;

    InputEntityMessage();
    InputEntityMessage(uint16_t entityId, float inputX, float inputY, float yaw, int sequenceId);

    uint16_t entityId = 0;
    float inputX = 0.0f;
    float inputY = 0.0f;
    float yaw = 0.0f;
    int sequenceId = 0;

    uint8_t getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // INPUT_ENTITY_MESSAGE_H
