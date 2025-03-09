#pragma once

#include <vector>
#include <iostream>
#include <winsock2.h>
#include "Server.h"

struct Message
{
    Message() = default; 

    virtual int getId() const = 0;
    virtual std::vector<uint8_t>& serialize(std::vector<uint8_t>& buffer) const = 0;
    virtual void deserialize(const std::vector<uint8_t>& buffer) = 0;
    virtual void process(const sockaddr_in& senderAddr) const = 0;

    virtual ~Message() = default;
};
