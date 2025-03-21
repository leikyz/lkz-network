#include "CreateClientMessage.h"

CreateClientMessage::CreateClientMessage() {}

int CreateClientMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& CreateClientMessage::serialize(Serializer& serializer) const
{
    serializer.writeInt(ID);
    return serializer.buffer;
}

void CreateClientMessage::deserialize(Deserializer& deserializer)
{
   
}

void CreateClientMessage::process(const sockaddr_in& senderAddr)
{
    ClientManager::addClient(senderAddr);

    Serializer serializer;
    serialize(serializer);

    Server::Send(senderAddr, serializer.buffer);
}
