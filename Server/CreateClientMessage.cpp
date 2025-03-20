#ifndef CREATE_CLIENT_MESSAGE_H
#define CREATE_CLIENT_MESSAGE_H

#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"

struct CreateClientMessage : public Message
{
    static constexpr int ID = 1;

    CreateClientMessage() {}

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(Serializer& serializer) const override
    {
        serializer.writeInt(ID);
        return serializer.buffer; 
    }

    void deserialize(Deserializer& deserializer) override
    {

    }

    void process(const sockaddr_in& senderAddr) override
    {
        ClientManager::addClient(senderAddr);

        Serializer serializer;
        serialize(serializer);  

        Server::Send(senderAddr, serializer.buffer);
    }
};

#endif // CREATE_CLIENT_MESSAGE_H
