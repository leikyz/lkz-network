#include "LKZ/Core/ECS/System/AISystem.h"
#include "LKZ/Core/ECS/Manager/ComponentManager.h"
#include "LKZ/Simulation/World.h"
#include "LKZ/Simulation/Math/MathUtils.h"
#include "LKZ/Simulation/Math/Vector.h"
#include <cmath>
#include <LKZ/Core/Engine.h>
#include <LKZ/Utility/Logger.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Protocol/Message/Entity/MoveEntitiesMessage.h>
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h"

#include "DetourCrowd.h"
#include "DetourNavMeshQuery.h"

#include <float.h> 

constexpr float aiMessageRate = 0.2f;
constexpr float aiRepathRate = 1.0f; 
constexpr float aiStopDistance = 2.0f; 
constexpr float aiStopDistanceSq = aiStopDistance * aiStopDistance; 

void AISystem::Update(ComponentManager& components, float deltaTime)
{
    static std::unordered_map<Entity, float> timeSinceLastSend;

    World& world = Engine::Instance().GetWorld();
    dtNavMesh* navMesh = world.getNavMesh();
    dtCrowd* crowd = world.getCrowd();
    if (!navMesh || !crowd) return;

    dtNavMeshQuery* navQuery = NavMeshQueryManager::GetThreadLocalQuery(navMesh);
    if (!navQuery) return;

    const dtQueryFilter* filter = crowd->getFilter(0); // Use filter 0 (default)

    std::unordered_map<Lobby*, MoveEntitiesMessage> lobbyMessages;

    for (auto& [entity, ai] : components.ai)
    {
        if (ai.crowdAgentIndex == -1) continue; // Not a crowd agent

        Vector3& position = components.positions[entity].position;
        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        ai.repathTimer -= deltaTime;
        bool shouldRepath = false;

        if (!ai.targetPosition.has_value() || ai.repathTimer <= 0.0f)
        {
            shouldRepath = true;
        }

        if (shouldRepath)
        {
            ai.repathTimer = aiRepathRate;

            // --- Find nearest player logic ---
            Entity nearestPlayerEntity = 0;
            float minDistanceSq = FLT_MAX;
            Vector3 targetPos;

            for (auto& [playerEntity, input] : components.playerInputs)
            {
                if (components.positions.find(playerEntity) == components.positions.end()) continue;
                Lobby* playerLobby = EntityManager::Instance().GetLobbyByEntity(playerEntity);
                if (playerLobby != lobby) continue;

                Vector3& playerPos = components.positions[playerEntity].position;
                float distSq = (playerPos - position).LengthSquared();

                if (distSq < minDistanceSq)
                {
                    minDistanceSq = distSq;
                    nearestPlayerEntity = playerEntity;
                    targetPos = playerPos;
                }
            }

            if (nearestPlayerEntity != 0)
            {
                if (minDistanceSq < aiStopDistanceSq)
                {
                    // We are close enough, STOP moving.
                    ai.targetPosition.reset(); // Clear our internal target
                    crowd->resetMoveTarget(ai.crowdAgentIndex); // Tell crowd to stop
                }
                else
                {
                    // We are far away, so pathfind to the player.
                    ai.targetPosition = targetPos;

                    // Find the nearest navmesh point to the player's position
                    const float extents[3] = { 10.0f, 10.0f, 10.0f }; // Search box
                    dtPolyRef targetRef;
                    float nearestPt[3];

                    navQuery->findNearestPoly(targetPos.data(), extents, filter, &targetRef, nearestPt, nullptr);

                    if (targetRef)
                    {
                        // Tell the crowd agent to move to this point
                        crowd->requestMoveTarget(ai.crowdAgentIndex, targetRef, nearestPt);
                    }
                }
            }
            else // No player was found
            {
                ai.targetPosition.reset();
                crowd->resetMoveTarget(ai.crowdAgentIndex);
            }
        }

        timeSinceLastSend[entity] += deltaTime;

        if (timeSinceLastSend[entity] >= aiMessageRate)
        {
            timeSinceLastSend[entity] = 0.0f;
            lobbyMessages[lobby].addUpdate(entity, position.x, position.y, position.z);
        }
    }

    for (auto& [lobby, msg] : lobbyMessages)
    {
        if (!msg.updates.empty())
        {
            Serializer s;
            msg.serialize(s);
            Engine::Instance().Server()->SendToMultiple(lobby->clients, s.getBuffer(), msg.getClassName());
        }
    }
}
