#include "LKZ/Core/ECS/Manager/EntityManager.h"
#include "LKZ/Core/ECS/Component/Component.h."
#include <iostream>

Entity EntityManager::CreateEntity(EntityType type, ComponentManager& components, Lobby* lobby)
{
    Entity id;
    if (!freeIDs.empty()) {
        id = freeIDs.front();
        freeIDs.pop();
    }
    else {
        id = nextID++;
    }

    entityLobbyMap[id] = lobby;

    return id;
}

void EntityManager::DestroyEntity(Entity entity)
{
    freeIDs.push(entity);
    entityLobbyMap.erase(entity);
}

Lobby* EntityManager::GetLobbyByEntity(Entity entity)
{
    auto it = entityLobbyMap.find(entity);
    if (it != entityLobbyMap.end()) {
        return it->second;
    }
    return nullptr;
}
