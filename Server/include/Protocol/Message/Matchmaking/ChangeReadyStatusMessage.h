#ifndef CHANGE_READY_STATUS_MESSAGE_H
#define CHANGE_READY_STATUS_MESSAGE_H

#include "Protocol/Message/Message.h"

struct ChangeReadyStatusMessage : public Message
{
    static constexpr int ID = 6;

    ChangeReadyStatusMessage();

    bool isReady;
	byte positionInLobby;

    byte getId() const override;

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;
};

#endif // CHANGE_READY_STATUS_MESSAGE_H

