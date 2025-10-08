#include "LKZ/Protocol/Message/Entity/MoveEntityMessage.h"

MoveEntityMessage::MoveEntityMessage(int entityId, float x, float y, float z)
{

    this->entityId = entityId;
    this->posX = x;
    this->posY = y;
    this->posZ = z;
}
MoveEntityMessage::MoveEntityMessage() {};

byte MoveEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& MoveEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(posX);
    serializer.writeFloat(posY);
    serializer.writeFloat(posZ);

 

    return serializer.getBuffer();
}

void MoveEntityMessage::deserialize(Deserializer& deserializer)
{
}


void MoveEntityMessage::process(const sockaddr_in& senderAddr)
{

}

