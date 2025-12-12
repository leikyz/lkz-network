#pragma once
#include <mutex>
#include <vector>
#include <string>

class ProfilerState
{
public:
    static ProfilerState& Instance()
    {
        static ProfilerState instance;
        return instance;
    }

    int connectedClientsCount = 0;

    void Reset()
    {
        connectedClientsCount = 0;
    }

private:
    ProfilerState() = default;
    ~ProfilerState() = default;
    ProfilerState(const ProfilerState&) = delete;
    ProfilerState& operator=(const ProfilerState&) = delete;
};