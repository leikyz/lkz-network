#include "LKZ/Protocol/Message/Entity/EntityDeadMessage.h"
#include <cstdlib> 
#include <ctime>   
#include <LKZ/Core/ECS/Manager/EntityManager.h>

EntityDeadMessage::EntityDeadMessage() {};

EntityDeadMessage::EntityDeadMessage(uint16_t entityId)
    : entityId(entityId)
{
}

uint8_t EntityDeadMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& EntityDeadMessage::serialize(Serializer& serializer) const
{
    serializer.reset();

    serializer.writeByte(ID);
    serializer.writeUInt16(entityId);

    return serializer.getBuffer();
}

void EntityDeadMessage::deserialize(Deserializer& deserializer)
{
}


void EntityDeadMessage::process(const sockaddr_in& senderAddr)
{


}