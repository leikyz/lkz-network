#include "LKZ/Core/ECS/System/Player/MovementSystem.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Utility/Logger.h>
#include <iostream>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <algorithm>
#include <math.h>
#include <LKZ/Protocol/Message/Entity/RotateEntityMessage.h>
float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
constexpr float mouseSensitivity = 0.25f; 
constexpr float minPitch = -40.0f;
constexpr float maxPitch = 80.0f;

void MovementSystem::Update(ComponentManager& components, float deltaTime)
{
    for (auto& [entity, input] : components.inputs)
    {
        // Ensure this entity also has mouse input and position
        if (components.mouseInputs.find(entity) == components.mouseInputs.end()) continue;
        if (components.positions.find(entity) == components.positions.end()) continue;

        auto& position = components.positions[entity];
        auto& mouseInput = components.mouseInputs[entity];

        // ----------- MOVEMENT -----------
        if (std::abs(input.inputX) > 0.001f || std::abs(input.inputY) > 0.001f)
        {
            float speed = 0.1f;
            float moveX = input.inputX * speed * deltaTime;
            float moveZ = input.inputY * speed * deltaTime;

            position.x += moveX;
            position.z += moveZ;

            Logger::Log("Entity " + std::to_string(entity) + " moved to X: " +
                std::to_string(position.x) + " Z: " + std::to_string(position.z), LogType::Debug);

            Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);

            // Uncomment when ready to sync to clients:
            /*
            MoveEntityMessage moveEntityMessage(entity, position.x, position.y, position.z);
            Serializer s;
            moveEntityMessage.serialize(s);
            Engine::Instance().Server()->SendToMultiple(lobby->clients, s.getBuffer(), moveEntityMessage.getClassName());
            */
        }

        if (std::abs(mouseInput.inputX) > 0.001f || std::abs(mouseInput.inputY) > 0.001f)
        {
            if (components.rotations.find(entity) == components.rotations.end())
                continue; 

            auto& rotation = components.rotations[entity];

            rotation.x += mouseInput.inputX * mouseSensitivity;

            if (rotation.x >= 360.f) rotation.x -= 360.f;
            if (rotation.x < 0.f) rotation.x += 360.f;

            // Update pitch (vertical rotation) and clamp
            rotation.y -= mouseInput.inputY * mouseSensitivity;
            rotation.y = std::clamp(rotation.y, minPitch, maxPitch);

            Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);


            RotateEntityMessage moveEntityMessage;
            moveEntityMessage.entityId = entity;
            moveEntityMessage.rotaY = rotation.x;
            Serializer s;
            moveEntityMessage.serialize(s);
            Engine::Instance().Server()->SendToMultiple(lobby->clients, s.getBuffer(), moveEntityMessage.getClassName());
            Logger::Log("Entity " + std::to_string(entity) +
                " rotated to rotX: " + std::to_string(rotation.x) +
                " rotY: " + std::to_string(moveEntityMessage.rotaY), LogType::Debug);
        }

    }
}
