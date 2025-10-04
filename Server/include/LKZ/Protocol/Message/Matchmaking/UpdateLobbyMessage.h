#ifndef UPDATE_LOBBY_MESSAGE_H
#define UPDATE_LOBBY_MESSAGE_H

#include "LKZ/Protocol/Message/Message.h"

class UpdateLobbyMessage : public Message
{
public:
    static constexpr byte ID = 8; 

    byte updatedLobbyPos;
	byte playersCount;
    std::vector<byte> playersInLobby; 

    UpdateLobbyMessage();

    byte getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
    void deserialize(Deserializer& deserializer) override;
    void process(const sockaddr_in& senderAddr) override;

};

#endif // UPDATE_LOBBY_MESSAGE_H