#include "EventManager.h"
#include <iostream>
#include "CreateClientMessage.h"
#include "ServerInformationsMessage.h"
#include "JoinLobbyMessage.h"
#include "CreateEntityMessage.h"
#include "LobbyListMessage.h"
#include "SynchronizeEntitiesMessage.h"
#include "MoveEntityMessage.h"
#include "RotateEntityMessage.h"
#include "DisconnectClientMessage.h"
#include "StartMatchmakingMessage.h"
EventManager::MessageHandler EventManager::messageHandlers[256] = { nullptr };

void EventManager::BindEvents()
{
    EventManager::registerHandler<CreateClientMessage>(1);
    EventManager::registerHandler<ServerInformationsMessage>(2);
    EventManager::registerHandler<DisconnectClientMessage>(3);
    EventManager::registerHandler<StartMatchmakingMessage>(4);
   /* EventManager::registerHandler<JoinLobbyMessage>(3);*/
  /*  EventManager::registerHandler<CreateEntityMessage>(4);
    EventManager::registerHandler<LobbyListMessage>(5);
    EventManager::registerHandler<SynchronizeEntitiesMessage>(6);
    EventManager::registerHandler<MoveEntityMessage>(7);
    EventManager::registerHandler<RotateEntityMessage>(8);*/
}

template<typename T>
void EventManager::registerHandler(uint8_t id) 
{
    messageHandlers[id] = &handleMessage<T>;
}

void EventManager::processMessage(std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{
    if (buffer.empty()) {
        return;
    }

    uint8_t id = buffer[0]; 

    if (!messageHandlers[id]) {
        std::cout << "failed: " << static_cast<int>(id) << std::endl;
        return;
    }

  /*  std::cout << "Processing message with ID: " << static_cast<int>(id) << std::endl;*/

    buffer.erase(buffer.begin()); 

    messageHandlers[id](buffer, senderAddr);
}

template<typename T>
void EventManager::handleMessage(const std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{
    T msg;
    Deserializer deserializer(buffer);
    msg.deserialize(deserializer);
    msg.process(senderAddr);

    const char* blueColor = "\033[38;5;32m";
    const char* resetColor = "\033[0m";

    std::cout << blueColor << "[RECEIVED] {"
        << typeid(msg).name() << "}" << resetColor << std::endl;
}
