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
constexpr float aiMoveSpeed = 2.0f; 


void AISystem::Update(ComponentManager& components, float deltaTime)
{
    for (auto& [entity, ai] : components.ai)
    {
        if (!ai.targetPosition.has_value())
            continue;
        Vector3& position = components.positions[entity].position;

        World& world = Engine::Instance().GetWorld();
        Vector3 randomSpawnPoint = world.getRandomNavMeshPoint();
       Logger::Log(
            std::format("[AISystem] Entity {} target: x={:.3f}, y={:.3f}, z={:.3f}",
                entity, position.x, position.y, position.z),
            LogType::Debug
	   );

        if (ai.path.empty())
        {
            ai.path = Engine::Instance().GetWorld().CalculatePath(components.positions[entity].position, /*ai.targetPosition.value()*/Vector3(randomSpawnPoint.x, randomSpawnPoint.y, randomSpawnPoint.z));
            ai.currentPathIndex = 0;
            if (ai.path.empty())
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
    }
}

