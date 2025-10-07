#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include "ComponentManager.h"
#include <queue>

/**
 * @brief Manages creation and destruction of entities.
 */
class EntityManager {
    Entity nextID = 1; // start at 1, 0 is invalid
    std::queue<Entity> freeIDs;

public:
    static EntityManager& Instance() {
        static EntityManager instance;
        return instance;
    }

    /**
     * @brief Creates an entity of a given type (Player, AI, etc.)
     * and automatically registers all its components.
     */
    Entity CreateEntity(EntityType type, ComponentManager& components);

    void DestroyEntity(Entity entity);

private:
    EntityManager() = default;
    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
};
