#pragma once
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

class ProfilerEventManager
{
public:
    using MessageHandler = void(*)(const std::vector<uint8_t>&);

    static void BindEvents();

    static void ProcessMessage(const std::vector<uint8_t>& buffer);

    template<typename T>
    static void RegisterHandler(uint8_t id);

    template<typename T>
    static void handleMessage(const std::vector<uint8_t>& buffer);

private:
    static MessageHandler handlers[256];
};