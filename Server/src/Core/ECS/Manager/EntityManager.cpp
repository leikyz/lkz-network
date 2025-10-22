#include "LKZ/Core/ECS/Manager/EntityManager.h"
#include "LKZ/Core/ECS/Component/Component.h."
#include <iostream>

Entity EntityManager::CreateEntity(EntitySuperType type, ComponentManager& components, Lobby* lobby)
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

Entity EntityManager::GetEntityById(uint32_t entityId, Lobby* lobby)
{

    for (const auto& [entity, mappedLobby] : entityLobbyMap) {
        if (entity == entityId && mappedLobby == lobby) {
            return entity;
        }
    }
    return 0; // or some invalid entity value
}

Lobby* EntityManager::GetLobbyByEntity(Entity entity)
{

    auto it = entityLobbyMap.find(entity);
    if (it != entityLobbyMap.end()) {
        return it->second;
    }
    return nullptr;
}

void EntityManager::SetLastSequenceId(Entity entity, uint32_t sequenceId) {

    lastSequenceIds[entity] = sequenceId;
}

uint32_t EntityManager::GetLastSequenceId(Entity entity) const {

    auto it = lastSequenceIds.find(entity);
    if (it != lastSequenceIds.end())
        return it->second;

    return 0; // default if not found
}

