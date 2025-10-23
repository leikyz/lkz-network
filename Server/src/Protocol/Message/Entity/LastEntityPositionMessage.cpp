#include "LKZ/Protocol/Message/Entity/LastEntityPositionMessage.h"
#include <cstdlib> 
#include <ctime>   

LastEntityPositionMessage::LastEntityPositionMessage() {};

LastEntityPositionMessage::LastEntityPositionMessage(uint16_t entityId, float posX, float posY, float posZ, uint32_t lastProcessedInput)
	: entityId(entityId), posX(posX), posY(posY), posZ(posZ), lastProcessedInput(lastProcessedInput)
{
}

uint8_t LastEntityPositionMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& LastEntityPositionMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeUInt16(entityId);
    serializer.writeFloat(posX);
    serializer.writeFloat(posY);
    serializer.writeFloat(posZ);
    serializer.writeUInt32(lastProcessedInput);

    return serializer.getBuffer();
}

void LastEntityPositionMessage::deserialize(Deserializer& deserializer)
{
}


void LastEntityPositionMessage::process(const sockaddr_in& senderAddr)
{
}

