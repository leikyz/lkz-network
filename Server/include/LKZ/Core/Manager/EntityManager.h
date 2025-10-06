#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include <queue>

/**
 * @brief Manages creation and destruction of entities.
 */
class EntityManager {
    Entity nextID = 1; // start at 1, 0 is invalid
    std::queue<Entity> freeIDs;

public:
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
};
