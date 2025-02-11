#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <functional>
#include <sstream>

class EventManager
{
public:
    EventManager() = delete;

    using EventCallback = std::function<void(std::vector<std::string>)>;

    static std::unordered_map<std::string, std::vector<EventCallback>> events;

    static void RegisterEvent(const std::string& name, EventCallback function);
    static void TriggerRaw(const std::string& message);
    static void Trigger(const std::string& clientId, const std::string& name, const std::vector<std::string>& args);

    // Serialize and deserialize methods
    static void Serialize();
    static void Deserialize(const std::vector<uint8_t>& buffer);

    // Writer
    static void WriteInt(int value);

    // Reader
    static void ReadInt(int value);
};

#endif // !1
