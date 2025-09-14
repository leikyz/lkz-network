//#ifndef SYNCHRONIZE_ENTITIES_MESSAGE_H
//#define SYNCHRONIZE_ENTITIES_MESSAGE_H
//
//#include "Message.h"
//
//struct SynchronizeEntitiesMessage : public Message
//{
//    static constexpr int ID = 6;
//
//    SynchronizeEntitiesMessage();
//
//    byte getId() const override;
//    std::vector<uint8_t>& serialize(Serializer& serializer) const override;
//    void deserialize(Deserializer& deserializer) override;
//    void process(const sockaddr_in& senderAddr) override;
//};
//
//#endif // SYNCHRONIZE_ENTITIES_MESSAGE_H
