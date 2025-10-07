#include "LKZ/Core/ECS/System/Player/MovementSystem.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Utility/Logger.h>
#include <iostream>

void MovementSystem::Update(ComponentManager& components, float deltaTime) 
{
    for (auto& [entity, position] : components.positions) 
    {
        if (components.types.find(entity) != components.types.end())
        {
            Logger::Log("Updating player entity position"); // Example usage of Logger
            auto& player = components.types[entity];
       /*     position.x += player.speed * deltaTime;*/
        }
    }
}
