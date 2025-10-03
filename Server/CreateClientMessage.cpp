#include "CreateClientMessage.h"

CreateClientMessage::CreateClientMessage() {}

byte CreateClientMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& CreateClientMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    return serializer.getBuffer();
}

void CreateClientMessage::deserialize(Deserializer& deserializer)
{
   
}

void CreateClientMessage::process(const sockaddr_in& senderAddr)
{
    ClientManager::addClient(senderAddr);

    Serializer serializer;
    serialize(serializer);

    Server::Send(senderAddr, serializer.getBuffer());
}
