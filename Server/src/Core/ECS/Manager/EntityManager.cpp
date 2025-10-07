#include "LKZ/Core/ECS/Manager/EntityManager.h"
#include "LKZ/Core/ECS/Component/Component.h."
#include <iostream>

Entity EntityManager::CreateEntity(EntityType type, ComponentManager& components)
{
    Entity entity;

    // Reuse free IDs if available
    if (!freeIDs.empty()) {
        entity = freeIDs.front();
        freeIDs.pop();
    }
    else {
        entity = nextID++;
    }

    // Assign common components
    components.types[entity] = TypeComponent{ type };
    components.positions[entity] = PositionComponent{ 0.0f, 0.0f, 0.0f };

    // Assign type-specific components
    switch (type)
    {
    case EntityType::Player:
        /*components.players[entity] = PlayerComponent{
            .health = 100.0f,
            .speed = 5.0f
        };*/
        std::cout << "[EntityManager] Player created (ID " << entity << ")\n";
        break;

    case EntityType::AI:
     /*   components.ais[entity] = AIComponent{
            .health = 50.0f,
            .speed = 2.0f,
            .behaviorState = 0
        };*/
        std::cout << "[EntityManager] AI created (ID " << entity << ")\n";
        break;

    default:
        std::cerr << "[EntityManager] Unknown entity type!\n";
        break;
    }

    return entity;
}

void EntityManager::DestroyEntity(Entity entity)
{
    freeIDs.push(entity);
}
