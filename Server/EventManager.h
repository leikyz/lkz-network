#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>
#include <winsock2.h>

class EventManager
{
public:
    using MessageHandler = void(*)(const std::vector<uint8_t>&, const sockaddr_in&);

    static void BindEvents();
    static void processMessage(std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr);

private:
    static MessageHandler messageHandlers[256];

    template<typename T>
    static void handleMessage(const std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr);

    template<typename T>
    static void registerHandler(int id);
};

#endif // EVENT_MANAGER_H