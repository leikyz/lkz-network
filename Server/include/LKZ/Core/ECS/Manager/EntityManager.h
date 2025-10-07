#pragma once
#include "LKZ/Session/Lobby.h"
#include "ComponentManager.h"
#include <queue>
#include <unordered_map>

class EntityManager {
    Entity nextID = 1; // start at 1, 0 is invalid
    std::queue<Entity> freeIDs;

    std::unordered_map<Entity, Lobby*> entityLobbyMap;

public:
    static EntityManager& Instance() {
        static EntityManager instance;
        return instance;
    }

    Entity CreateEntity(EntityType type, ComponentManager& components, Lobby* lobby);

    void DestroyEntity(Entity entity);

    Lobby* GetLobbyByEntity(Entity entity);

private:
    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
};
