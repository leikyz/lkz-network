//#include "EventManager.h"
//#include <sstream>
//#include <iostream>
//
//std::unordered_map<int, std::vector<EventManager::EventCallback>> EventManager::events;
//
//void EventManager::RegisterEvent(int eventId, EventCallback function) {
//    events[eventId].push_back(function);
//}
//
//void EventManager::TriggerRaw(BaseClient* client, const std::string& message) {
//    std::vector<std::string> content = Deserialize(message);
//
//    if (content.empty()) {
//        std::cerr << "Invalid event call" << std::endl;
//        return;
//    }
//
//    int eventId = std::stoi(content[0]); // Convertit l'ID en entier
//    std::vector<std::string> args(content.begin() + 1, content.end());
//
//    if (events.find(eventId) != events.end()) {
//        for (const auto& function : events[eventId]) {
//            function(client, args);
//        }
//    }
//}
//
//std::string EventManager::Serialize(int eventId, const std::vector<std::string>& parameters) {
//    std::ostringstream oss;
//    oss << eventId << "|";
//
//    if (!parameters.empty()) {
//        for (size_t i = 0; i < parameters.size(); ++i) {
//            oss << parameters[i];
//            if (i < parameters.size() - 1) {
//                oss << "&";
//            }
//        }
//    }
//
//    oss << "~";
//    return oss.str();
//}
//
//std::vector<std::string> EventManager::Deserialize(const std::string& message) {
//    std::vector<std::string> result;
//    size_t separator = message.find('|');
//
//    if (separator == std::string::npos) {
//        return result;
//    }
//
//    std::string eventIdStr = message.substr(0, separator);
//    std::string paramString = message.substr(separator + 1);
//
//    result.push_back(eventIdStr); // Conserve l'ID sous forme de string pour la conversion
//
//    if (!paramString.empty() && paramString != "~") {
//        size_t start = 0, end;
//        while ((end = paramString.find('&', start)) != std::string::npos) {
//            result.push_back(paramString.substr(start, end - start));
//            start = end + 1;
//        }
//        result.push_back(paramString.substr(start));
//    }
//
//    return result;
//}
//
//bool EventManager::ValidateParameters(const std::vector<std::string>& parameters, size_t expectedCount) {
//    return parameters.size() == expectedCount;
//}
