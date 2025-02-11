//#ifndef EVENT_MANAGER_H
//
//class EventManager
//{
//public:
//
//    EventManager() = delete;
//
//    using EventCallback = std::function<void(std::vector<std::string>)>;
//
//    static std::unordered_map<std::string, std::vector<EventCallback>> events; // Déclaration ici !
//
//    static void HandleOnlinePlayerCountMessage(const std::string& name, EventCallback function);
//    static void TriggerRaw(const std::string& message);
//    static void Trigger(const std::string& clientId, const std::string& name, const std::vector<std::string>& args);
//};
//#endif // !1