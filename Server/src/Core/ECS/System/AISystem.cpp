#include "LKZ/Core/ECS/Manager/ComponentManager.h"
#include "LKZ/Simulation/World.h"
#include "LKZ/Simulation/Math/MathUtils.h" 
#include "LKZ/Simulation/Math/Vector.h" 
#include <cmath> 
#include <LKZ/Core/ECS/System/AISystem.h>
#include <LKZ/Core/Engine.h>
#include "Recast.h"
#include "RecastAlloc.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h" 
#include "DetourNavMeshQuery.h" 
#include "DetourCommon.h"
#include <LKZ/Utility/Logger.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h" 

constexpr float aiMoveSpeed = 3.0f;
constexpr float aiMessageRate = 0.2f; 
constexpr float aiRepathRate = 1.0f;  

void AISystem::Update(ComponentManager& components, float deltaTime)
{
    static std::unordered_map<Entity, float> timeSinceLastSend;

    World& world = Engine::Instance().GetWorld();
    dtNavMesh* navMesh = world.getNavMesh();
    if (!navMesh) return;

    dtNavMeshQuery* navQuery = NavMeshQueryManager::GetThreadLocalQuery(navMesh);
    if (!navQuery) return;

    for (auto& [entity, ai] : components.ai)
    {
        if (!ai.targetPosition.has_value())
            continue;

        Vector3& position = components.positions[entity].position;
        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        ai.repathTimer -= deltaTime;
        bool shouldRepath = false;

        if (ai.path.empty())
        {
            shouldRepath = true;
        }
        else if (ai.repathTimer <= 0.0f)
        {
            shouldRepath = true;
            ai.repathTimer = aiRepathRate;
        }

        if (shouldRepath)
        {
            Entity targetEntity = EntityManager::Instance().GetEntityById(1, lobby);
            if (components.positions.find(targetEntity) == components.positions.end())
            {
                ai.path.clear();
                ai.targetPosition.reset();
                continue;
            }

            Vector3& targetPos = components.positions[targetEntity].position;

            ai.path = world.CalculatePath(navQuery, position, targetPos);
            ai.currentPathIndex = 0;
            if (ai.path.empty())
                continue;
        }

        if (ai.path.empty())
        {
            continue;
        }

        Vector3& target = ai.path[ai.currentPathIndex];

        Vector3 dir = target - position;
        float dist = dir.Length();
        if (dist < 0.05f)
        {
            ai.currentPathIndex++;
            if (ai.currentPathIndex >= ai.path.size())
            {
                ai.path.clear();
                ai.targetPosition.reset();
                continue;
            }
            target = ai.path[ai.currentPathIndex];
            dir = target - position;
            dist = dir.Length();
        }

        dir.Normalize();
        position += dir * aiMoveSpeed * deltaTime;

        float yaw = std::atan2(dir.x, dir.z) * 180.0f / 3.14159265f;
        components.rotations[entity].rotation.y = yaw;

        timeSinceLastSend[entity] += deltaTime;

        if (timeSinceLastSend[entity] >= aiMessageRate)
        {
            timeSinceLastSend[entity] = 0.0f;

            MoveEntityMessage moveMsg(entity, position.x, position.y, position.z);
            Serializer s;
            moveMsg.serialize(s);
            Engine::Instance().Server()->SendToMultiple(
                lobby->clients, s.getBuffer(), moveMsg.getClassName()
            );
        }
    }
}
