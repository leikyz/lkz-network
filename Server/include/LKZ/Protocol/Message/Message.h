#pragma once

#include <winsock2.h>
#include "LKZ/Protocol/Codec/Serializer.h"
#include "LKZ/Protocol/Codec/Deserializer.h"
#include "LKZ/Core/Server.h"
#include "LKZ/Manager/ClientManager.h"
#include "LKZ/Manager/LobbyManager.h"
#include "LKZ/Core/Engine.h"
struct Message
{
    Message() = default;

    virtual uint8_t getId() const = 0;
    virtual std::vector<uint8_t>& serialize(Serializer& serializer) const = 0;
    virtual void deserialize(Deserializer& deserializer) = 0;
    virtual void process(const sockaddr_in& senderAddr)  = 0;

    virtual ~Message() = default;
};
