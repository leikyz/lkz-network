#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>
#include <winsock2.h>
#include "Deserializer.h"

class EventManager
{
public:
    using MessageHandler = std::function<void(Deserializer&, const sockaddr_in&)>;

    static void BindEvents();
    static void processMessage(const std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr);

private:
    static std::unordered_map<int, MessageHandler> messageHandlers;

    template<typename T>
    static void handleMessage(Deserializer& deserializer, const sockaddr_in& senderAddr);

    template<typename T>
    static void registerHandler(int id);
};

#endif // EVENT_MANAGER_H
