#include "EventManager.h"
#include <iostream>
#include "CreateClientMessage.cpp"

EventManager::MessageHandler EventManager::messageHandlers[256] = { nullptr };

void EventManager::BindEvents()
{
    EventManager::registerHandler<CreateClientMessage>(1);
}

// Register handler outside of the class definition
template<typename T>
void EventManager::registerHandler(int id)
{
    if (id >= 0 && id < 256)
    {
        messageHandlers[id] = &handleMessage<T>;
    }
}

void EventManager::processMessage(std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{
    if (buffer.empty()) {
        return;
    }

    int id = static_cast<int>(buffer[0]);

    // Check if it's in range
    if (id < 0 || id >= 256 || !messageHandlers[id])
        return;

    buffer.erase(buffer.begin());

    messageHandlers[id](buffer, senderAddr);
}

// Define the template function outside of the class definition
template<typename T>
void EventManager::handleMessage(const std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{
    T msg;
    msg.deserialize(buffer);
    msg.process(senderAddr);

    // ANSI escape code for yellow color
    const char* yellowColor = "\033[38;5;226m"; // This is a yellow color
    const char* resetColor = "\033[0m"; // Reset the color

    std::cout << yellowColor << "Message received: {"
        << typeid(msg).name() << "}" << resetColor << std::endl;
}


