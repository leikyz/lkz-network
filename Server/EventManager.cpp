#include "EventManager.h"
#include <iostream>
#include "CreateClientMessage.cpp"
#include "CreateLobbyMessage.cpp"

std::unordered_map<int, EventManager::MessageHandler> EventManager::messageHandlers;

void EventManager::BindEvents()
{
    registerHandler<CreateClientMessage>(1);
    registerHandler<CreateLobbyMessage>(2);
}

template<typename T>
void EventManager::registerHandler(int id)
{
    if (id >= 0)
    {
        messageHandlers[id] = [](Deserializer& deserializer, const sockaddr_in& senderAddr) {
            handleMessage<T>(deserializer, senderAddr);
            };
    }
}

void EventManager::processMessage(const std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{
    if (buffer.empty()) return;

    Deserializer deserializer(buffer);

    int id = deserializer.readInt();  

    auto it = messageHandlers.find(id);
    if (it != messageHandlers.end()) {
        it->second(deserializer, senderAddr); 
    }
}

template<typename T>
void EventManager::handleMessage(Deserializer& deserializer, const sockaddr_in& senderAddr)
{
    T msg;
    msg.deserialize(deserializer);  
    msg.process(senderAddr);

    std::cout << "\033[38;5;226mMessage reçu : {" << typeid(msg).name() << "}" << std::endl;
}
