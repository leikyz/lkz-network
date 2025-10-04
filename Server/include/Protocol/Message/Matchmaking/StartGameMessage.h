#ifndef START_GAME_MESSAGE_H
#define START_GAME_MESSAGE_H

#include "Protocol/Message/Message.h"

struct StartGameMessage : public Message
{
    static constexpr int ID = 5;

    StartGameMessage();

    byte mapId;

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // START_GAME_MESSAGE_H

