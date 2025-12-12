#pragma once
#include <cstdint>
#include <vector>
#include <Common/Codec/Serializer.h>
#include <Common/Codec/Deserializer.h>


struct ProfilerMessage
{
    ProfilerMessage() = default;

    virtual uint8_t getId() const = 0;
    virtual void deserialize(Deserializer& deserializer) = 0;
    virtual void process() = 0;

    virtual ~ProfilerMessage() = default;
};
