#include "LKZ/Core/ECS/System/Player/MovementSystem.h"
#include <iostream>

void MovementSystem::Update(ComponentManager& components, float deltaTime) 
{
	
   std::cout << "[MAIN] Delta Time: " << deltaTime << " seconds\n";
    for (auto& [entity, position] : components.positions) 
    {
        if (components.players.find(entity) != components.players.end()) 
        {
            auto& player = components.players[entity];
            position.x += player.speed * deltaTime;
        }
        if (components.ais.find(entity) != components.ais.end()) 
        {
            auto& ai = components.ais[entity];
            position.x += ai.speed * deltaTime;
        }
    }
}
