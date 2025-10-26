﻿
#include <iostream>
#include "LKZ/Core/Manager/EventManager.h"
#include "LKZ/Protocol/Message/Approach/CreateClientMessage.h"
#include "LKZ/Protocol/Message/Approach/ServerInformationsMessage.h"
#include "LKZ/Protocol/Message/Approach/DisconnectClientMessage.h"
#include "LKZ/Protocol/Message/Entity/CreateEntityMessage.h"
#include "LKZ/Protocol/Message/Entity/LastEntityPositionMessage.h"
#include "LKZ/Protocol/Message/Entity/MoveEntityMessage.h"
#include "LKZ/Protocol/Message/Entity/RotateEntityMessage.h"
#include "LKZ/Protocol/Message/Entity/Player/PlayerInputMessage.h"
#include "LKZ/Protocol/Message/Entity/Player/PlayerStateMessage.h"
#include "LKZ/Protocol/Message/Entity/CreateEntityMessage.h"
#include "LKZ/Protocol/Message/Entity/MoveEntityMessage.h"
#include "LKZ/Protocol/Message/Entity/RotateEntityMessage.h"
#include "LKZ/Protocol/Message/Matchmaking/StartMatchmakingMessage.h"
#include "LKZ/Protocol/Message/Matchmaking/ChangeReadyStatusMessage.h"
#include "LKZ/Protocol/Message/Matchmaking/LeaveLobbyMessage.h"
#include "LKZ/Protocol/Message/Matchmaking/UpdateLobbyMessage.h"
#include "LKZ/Protocol/Message/Matchmaking/StartGameMessage.h"
#include "LKZ/Protocol/Message/Entity/RequestCreateEntityMessage.h"
#include <LKZ/Protocol/Message/Entity/MoveEntitiesMessage.h>


EventManager::MessageHandler EventManager::messageHandlers[256] = { nullptr };

void EventManager::BindEvents()
{
    std::cout << "[EventManager] Initialize events." << std::endl;   
    EventManager::registerHandler<CreateClientMessage>(1);
    EventManager::registerHandler<ServerInformationsMessage>(2);
    EventManager::registerHandler<DisconnectClientMessage>(3);
    EventManager::registerHandler<StartMatchmakingMessage>(4);
    EventManager::registerHandler<StartGameMessage>(5);
    EventManager::registerHandler<ChangeReadyStatusMessage>(6);
    EventManager::registerHandler<LeaveLobbyMessage>(7);
    EventManager::registerHandler<UpdateLobbyMessage>(8);
    EventManager::registerHandler<CreateEntityMessage>(9);
    EventManager::registerHandler<PlayerInputMessage>(10);
    EventManager::registerHandler<MoveEntityMessage>(11);
    EventManager::registerHandler<RotateEntityMessage>(12);
    EventManager::registerHandler<RotateEntityMessage>(12);
    EventManager::registerHandler<LastEntityPositionMessage>(13);
    EventManager::registerHandler<RequestCreateEntityMessage>(14);
    EventManager::registerHandler<MoveEntitiesMessage>(15);
    EventManager::registerHandler<PlayerStateMessage>(16);
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


    buffer.erase(buffer.begin()); 

    messageHandlers[id](buffer, senderAddr);
}

template<typename T>
void EventManager::handleMessage(const std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{ 
    T msg;
    Deserializer deserializer(buffer);

    std::string name = typeid(msg).name();
    name = name.substr(7); 

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(senderAddr.sin_addr), ip, INET_ADDRSTRLEN);
    // added +1 because we removed 1 byte before handled event (ID)
    Logger::Log(std::format("{} ({} bytes) [{}:{}]", name, buffer.size() + 1, ip, ntohs(senderAddr.sin_port)), LogType::Received);

    msg.deserialize(deserializer);
    msg.process(senderAddr);
}
