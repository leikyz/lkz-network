#ifndef PROFILER_NETWORK_PERFORMANCE_MESSAGE_H
#define PROFILER_NETWORK_PERFORMANCE_MESSAGE_H

#include "Profiler/include/Protocol/Message/ProfilerMessage.h"

struct ProfilerNetworkPerformanceMessage : public ProfilerMessage
{
    static constexpr uint8_t ID = 24;

    ProfilerNetworkPerformanceMessage();

    uint8_t getId() const override;

    float deltaTime;
    float fps;

    void deserialize(Deserializer& deserializer) override;
    void process() override;
};

#endif // PROFILER_NETWORK_PERFORMANCE_MESSAGE_H
