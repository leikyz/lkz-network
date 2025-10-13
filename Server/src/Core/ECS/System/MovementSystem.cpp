#include "LKZ/Core/ECS/System/Player/MovementSystem.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Utility/Logger.h>
#include <iostream>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <algorithm>
#include <math.h>
#include <LKZ/Protocol/Message/Entity/RotateEntityMessage.h>
#include <LKZ/Simulation/Math/MathUtils.h>

#include <unordered_map>
#include <LKZ/Simulation/Math/Vector.h>

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
constexpr float mouseSensitivity = 0.25f;
constexpr float minPitch = -40.0f;
constexpr float maxPitch = 80.0f;

constexpr float moveSpeed = 0.04f; // ajustable

void MovementSystem::Update(ComponentManager& components, float deltaTime)
{
    static std::unordered_map<Entity, Vector3> lastSentPositions;
    const float moveThreshold = 0.05f; // Adjust sensitivity (meters)

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

        Vector3 currentPos = { position.x, position.y, position.z };
        auto it = lastSentPositions.find(entity);

        bool shouldSend = false;
     /*   if (it == lastSentPositions.end())
        {
            shouldSend = true;
            lastSentPositions[entity] = currentPos;
        }
        else
        {
            Vector3 lastPos = it->second;
            float distSq = MathUtils::Distance(currentPos, lastPos);
            if (distSq > moveThreshold * moveThreshold)
            {
                shouldSend = true;
                lastSentPositions[entity] = currentPos;
            }
        }*/

       /* if (shouldSend)
        {*/
          /*  Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
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
            }*/
       /* }*/
    }
}


