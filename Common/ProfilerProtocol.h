#pragma once
#include <cstdint>

namespace ProfilerProtocol
{
    static const int PROFILER_PORT = 5001;

    enum PacketID : uint8_t
    {
        Handshake = 255,
        TestMessage = 1,
        FrameData = 2,
        NavMeshChunk = 3
    };
}