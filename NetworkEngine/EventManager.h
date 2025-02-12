#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

class BaseClient; // Déclaration avant utilisation

class EventManager {
public:
    using EventCallback = std::function<void(BaseClient*, const std::vector<std::string>&)>;

    static void RegisterEvent(int eventId, EventCallback function);
    static void TriggerRaw(BaseClient* client, const std::string& message);
    static std::string Serialize(int eventId, const std::vector<std::string>& parameters);
    static std::vector<std::string> Deserialize(const std::string& message);
    static bool ValidateParameters(const std::vector<std::string>& parameters, size_t expectedCount);

private:
    static std::unordered_map<int, std::vector<EventCallback>> events;
};

#endif // EVENT_MANAGER_H
