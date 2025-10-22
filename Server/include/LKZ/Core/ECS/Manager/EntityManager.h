#pragma once
#include "LKZ/Session/Lobby.h"
#include "ComponentManager.h"
#include <queue>
#include <unordered_map>
#include <mutex> 

class EntityManager {
    Entity nextID = 1; // start at 1, 0 is invalid
    std::queue<Entity> freeIDs;

    std::unordered_map<Entity, Lobby*> entityLobbyMap;

public:
    static EntityManager& Instance() {
        static EntityManager instance;
        return instance;
    }

    void SetLastSequenceId(Entity entity, uint32_t sequenceId);
    uint32_t GetLastSequenceId(Entity entity) const;

    Entity CreateEntity(EntitySuperType type, ComponentManager& components, Lobby* lobby);

    void DestroyEntity(Entity entity);

    Lobby* GetLobbyByEntity(Entity entity);

    Entity GetEntityById(uint32_t entityId, Lobby* lobby);

private:
    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    std::unordered_map<Entity, uint32_t> lastSequenceIds;
    mutable std::mutex m_mutex;
};
