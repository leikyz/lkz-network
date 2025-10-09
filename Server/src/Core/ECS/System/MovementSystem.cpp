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

constexpr float moveSpeed = 0.1f; // ajustable


void MovementSystem::Update(ComponentManager& components, float deltaTime)
{
    for (auto& [entity, input] : components.inputs)
    {
        if (components.positions.find(entity) == components.positions.end()) continue;
        if (components.rotations.find(entity) == components.rotations.end()) continue;

        auto& position = components.positions[entity];

        float cameraYaw = components.rotations[entity].y;
        float yawRad = cameraYaw * (3.14159265f / 180.0f);

        float forwardX = std::sin(yawRad);
        float forwardZ = std::cos(yawRad);
        float rightX = std::cos(yawRad);
        float rightZ = -std::sin(yawRad);

        float dirX = rightX * input.inputX + forwardX * input.inputY;
        float dirZ = rightZ * input.inputX + forwardZ * input.inputY;

        float len = std::sqrt(dirX * dirX + dirZ * dirZ);
        if (len > 0.001f)
        {
            dirX /= len;
            dirZ /= len;
        }

        position.x += dirX * moveSpeed * deltaTime;
        position.z += dirZ * moveSpeed * deltaTime;


        position.x += dirX * moveSpeed * deltaTime;
        position.z += dirZ * moveSpeed * deltaTime;

        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (lobby)
        {
            MoveEntityMessage moveEntityMessage(entity, position.x, position.y, position.z);
            Serializer s;
            moveEntityMessage.serialize(s);
            Engine::Instance().Server()->SendToMultiple(
                lobby->clients,
                s.getBuffer(),
                moveEntityMessage.getClassName()
            );
        }
    }
}

