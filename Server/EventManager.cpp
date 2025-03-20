#include "EventManager.h"
#include <iostream>
#include "CreateClientMessage.cpp"
#include "CreateLobbyMessage.cpp"
#include "JoinLobbyMessage.cpp";
#include "CreateEntityMessage.cpp";
EventManager::MessageHandler EventManager::messageHandlers[256] = { nullptr };

void EventManager::BindEvents()
{
    EventManager::registerHandler<CreateClientMessage>(1);
    EventManager::registerHandler<CreateLobbyMessage>(2);
    EventManager::registerHandler<JoinLobbyMessage>(3);
    EventManager::registerHandler<CreateEntityMessage>(4);
}

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
    if (buffer.size() < 4) {
        return;
    }

    int id = buffer[0] |
        (buffer[1] << 8) |
        (buffer[2] << 16) |
        (buffer[3] << 24);

    if (id < 0 || id >= 256 || !messageHandlers[id]) {
        return;
    }

    buffer.erase(buffer.begin(), buffer.begin() + 4);

    messageHandlers[id](buffer, senderAddr);
}


template<typename T>
void EventManager::handleMessage(const std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{
    T msg;
    Deserializer deserializer(buffer);
    msg.deserialize(deserializer);
    msg.process(senderAddr);

    const char* yellowColor = "\033[38;5;226m"; // This is a yellow color

    std::cout << yellowColor << "Message received: {"
        << typeid(msg).name() << "}" << std::endl;
}

