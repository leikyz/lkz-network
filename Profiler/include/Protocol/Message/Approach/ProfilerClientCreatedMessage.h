#ifndef PROFILER_CLIENT_CREATED_MESSAGE_H
#define PROFILER_CLIENT_CREATED_MESSAGE_H

#include "Profiler/include/Protocol/Message/ProfilerMessage.h"

struct ProfilerClientCreatedMessage : public ProfilerMessage
{
    static constexpr uint8_t ID = 23;

    ProfilerClientCreatedMessage();

    uint8_t getId() const override;

    void deserialize(Deserializer& deserializer) override;
    void process() override;
};

#endif // PROFILER_CLIENT_CREATED_MESSAGE_H
