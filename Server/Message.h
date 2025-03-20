#pragma once

#include <vector>
#include <winsock2.h>
#include "Serializer.h"
#include "Deserializer.h"

struct Message
{
    Message() = default;

    virtual int getId() const = 0;
    virtual std::vector<uint8_t>& serialize(Serializer& serializer) const = 0;
    virtual void deserialize(Deserializer& deserializer) = 0;
    virtual void process(const sockaddr_in& senderAddr) const = 0;

    virtual ~Message() = default;
};
