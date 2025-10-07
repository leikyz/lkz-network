#include "LKZ/Core/ECS/System/Player/MovementSystem.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Utility/Logger.h>
#include <iostream>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
void MovementSystem::Update(ComponentManager& components, float deltaTime)
{
    for (auto& [entity, position] : components.positions)
    {
        if (components.types.find(entity) != components.types.end() &&
            components.inputs.find(entity) != components.inputs.end())
        {
            auto& input = components.inputs[entity];
            auto& type = components.types[entity];

            if (input.inputX > 0.0f || input.inputY > 0.0f)
            {

                float targetX = position.x + 0.1f * deltaTime;
                float targetY = position.y + 0.1f * deltaTime;

                // Move based on input
                const float lerpFactor = 10.0f * deltaTime;
                position.x = position.x + (targetX - position.x) * lerpFactor;
                position.y = position.y + (targetY - position.y) * lerpFactor;

                Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);

                MoveEntityMessage moveEntityMessage(entity, position.x, position.y, position.z);
                Serializer s;
                moveEntityMessage.serialize(s);

                Engine::Instance().Server()->SendToMultiple(lobby->clients, s.getBuffer(), moveEntityMessage.getClassName());
            }

        }
    }
}

