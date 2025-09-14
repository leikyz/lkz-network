//#ifndef JOIN_LOBBY_MESSAGE_H
//#define JOIN_LOBBY_MESSAGE_H
//
//#include "Message.h"
//
//struct JoinLobbyMessage : public Message
//{
//    static constexpr int ID = 3;
//
//    JoinLobbyMessage();
//
//    int lobbyId;
//
//    byte getId() const override;
//
//    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
//    void deserialize(Deserializer& deserializer) override;
//    void process(const sockaddr_in& senderAddr) override;
//};
//
//#endif // JOIN_LOBBY_MESSAGE_H
