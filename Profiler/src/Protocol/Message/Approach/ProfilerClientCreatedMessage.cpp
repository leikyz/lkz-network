#include "Profiler/include/Protocol/Message/Approach/ProfilerClientCreatedMessage.h"
#include <Profiler/include/Core/Manager/ProfilerState.h>

ProfilerClientCreatedMessage::ProfilerClientCreatedMessage() {}

uint8_t ProfilerClientCreatedMessage::getId() const
{
    return ID;
}

void ProfilerClientCreatedMessage::deserialize(Deserializer& deserializer)
{

}

void ProfilerClientCreatedMessage::process()
{
    ProfilerState::Instance().connectedClientsCount++;
}
