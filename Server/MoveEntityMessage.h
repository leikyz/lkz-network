#ifndef MOVE_ENTITY_MESSAGE_H
#define MOVE_ENTITY_MESSAGE_H

#include "Message.h"

struct MoveEntityMessage : public Message
{
    static constexpr int ID = 7;

    MoveEntityMessage();

    MoveEntityMessage(int entityId, float inputX, float inputY);

    int entityId;

    float inputX;
    float inputY;

    int getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // MOVE_ENTITY_MESSAGE_H
