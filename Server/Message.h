#pragma once

#include <winsock2.h>
#include "Serializer.h"
#include "Deserializer.h"
#include "Server.h"
#include "ClientManager.h"
#include "LobbyManager.h"

struct Message
{
    Message() = default;

    virtual byte getId() const = 0;
    virtual std::vector<uint8_t>& serialize(Serializer& serializer) const = 0;
    virtual void deserialize(Deserializer& deserializer) = 0;
    virtual void process(const sockaddr_in& senderAddr)  = 0;

    virtual ~Message() = default;
};
