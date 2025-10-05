#include "LKZ/Protocol/Message/Approach/CreateClientMessage.h"

CreateClientMessage::CreateClientMessage() {}

uint8_t CreateClientMessage::getId() const
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

	Client* client = ClientManager::getClientByAddress(senderAddr);

    Serializer serializer;
    serialize(serializer);


        Engine::Instance().Server()->Send(senderAddr, serializer.getBuffer(), getClassName());

}
