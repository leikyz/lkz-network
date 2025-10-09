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
    for (auto& [entity, inputComponent] : components.inputs)
    {
        if (components.positions.find(entity) == components.positions.end()) continue;
        if (components.rotations.find(entity) == components.rotations.end()) continue;

        auto& positionComponent = components.positions[entity];
        auto& rotationComponent = components.rotations[entity];

        float cameraYaw = Quaternion::QuaternionToEuler(rotationComponent.rotation).y;
        float yawRad = cameraYaw * (3.14159265f / 180.0f);

        Logger::Log("Yaw : " + std::to_string(cameraYaw));

        float forwardX = std::sin(yawRad);
        float forwardZ = std::cos(yawRad);
        float rightX = std::cos(yawRad);
        float rightZ = -std::sin(yawRad);

        float dirX = rightX * inputComponent.input.x + forwardX * inputComponent.input.y;
        float dirZ = rightZ * inputComponent.input.x + forwardZ * inputComponent.input.y;

        float len = std::sqrt(dirX * dirX + dirZ * dirZ);
        if (len > 0.001f)
        {
            dirX /= len;
            dirZ /= len;
        }

        positionComponent.position.x += dirX * moveSpeed * deltaTime;
        positionComponent.position.z += dirZ * moveSpeed * deltaTime;


        positionComponent.position.x += dirX * moveSpeed * deltaTime;
        positionComponent.position.z += dirZ * moveSpeed * deltaTime;

        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (lobby)
        {
            MoveEntityMessage moveEntityMessage(entity, positionComponent.position.x, positionComponent.position.y, positionComponent.position.z);
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

