#include "Profiler/include/Protocol/Message/Approach/ProfilerNetworkPerformanceMessage.h"
#include <Profiler/include/Core/Manager/ProfilerState.h>

ProfilerNetworkPerformanceMessage::ProfilerNetworkPerformanceMessage() {}

uint8_t ProfilerNetworkPerformanceMessage::getId() const
{
    return ID;
}

void ProfilerNetworkPerformanceMessage::deserialize(Deserializer& deserializer)
{
	deltaTime = deserializer.readFloat();
	fps = deserializer.readFloat();
}

void ProfilerNetworkPerformanceMessage::process()
{
	ProfilerState::Instance().serverDeltaTime = deltaTime;
	ProfilerState::Instance().serverFps = fps;
}
