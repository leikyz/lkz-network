#include "LKZ/Core/ECS/System/AISystem.h"
#include "LKZ/Core/ECS/Manager/ComponentManager.h"
#include "LKZ/Simulation/World.h"
#include "LKZ/Simulation/Math/MathUtils.h"
#include "LKZ/Simulation/Math/Vector.h"
#include <cmath>
#include <LKZ/Core/Engine.h>
#include <LKZ/Utility/Logger.h>
#include <LKZ/Utility/Constants.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Protocol/Message/Entity/MoveEntitiesMessage.h>
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h"

#include "DetourCrowd.h"
#include "DetourNavMeshQuery.h"

#include <float.h> 
#include <DetourCommon.h>
#include <stdlib.h> 
#include <chrono>

void AISystem::Update(ComponentManager& components, float deltaTime)
{
    World& world = Engine::Instance().GetWorld();
    dtNavMesh* navMesh = world.getNavMesh();
    dtCrowd* crowd = world.getCrowd();
    if (!navMesh || !crowd) return;

    dtNavMeshQuery* navQuery = NavMeshQueryManager::GetThreadLocalQuery(navMesh);
    if (!navQuery) return;

    const dtQueryFilter* filter = crowd->getFilter(Constants::AGENT_QUERY_FILTER_TYPE);

    std::unordered_map<Lobby*, MoveEntitiesMessage> lobbyMessages;

    const float AI_AGGRO_RANGE_SQ = 25.0f * 25.0f; // 50 meters Aggro

    // --- SETTINGS: STOP DISTANCE ---
    // Zombies will stop moving if they are within this distance of the player.
    // 1.5f = 1.5 meters (Adjust this if they are still too close or too far)
    const float STOP_DISTANCE = 3.0f;
    const float STOP_DISTANCE_SQ = STOP_DISTANCE * STOP_DISTANCE;

    const int MAX_PATH_UPDATES_PER_TICK = 10;
    int pathUpdatesThisTick = 0;

    for (auto& [entity, ai] : components.ai)
    {
        if (ai.crowdAgentIndex == -1) continue;

        Vector3& position = components.positions[entity].position;
        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        ai.repathTimer -= deltaTime;

        bool allowedToPathfind = (pathUpdatesThisTick < MAX_PATH_UPDATES_PER_TICK);
        bool needsRepath = (ai.repathTimer <= 0.0f);

        if (allowedToPathfind && needsRepath)
        {
            pathUpdatesThisTick++;
            ai.repathTimer = 1.0f + ((rand() % 50) / 100.0f);

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

            if (nearestPlayerEntity != 0 && minDistanceSq < AI_AGGRO_RANGE_SQ)
            {
                // --- FIX: STOP DISTANCE CHECK ---
                // If we are closer than STOP_DISTANCE, reset target and stop moving.
                if (minDistanceSq < STOP_DISTANCE_SQ)
                {
                    ai.targetPosition.reset();
                    crowd->resetMoveTarget(ai.crowdAgentIndex);
                }
                else
                {
                    // Player is far enough, keep chasing
                    ai.targetPosition = targetPos;

                    const float extents[3] = { 10.0f, 10.0f, 10.0f };
                    dtPolyRef targetRef;
                    float nearestPt[3];

                    dtStatus status = navQuery->findNearestPoly(targetPos.data(), extents, filter, &targetRef, nearestPt, nullptr);

                    if (dtStatusSucceed(status) && targetRef)
                    {
                        crowd->requestMoveTarget(ai.crowdAgentIndex, targetRef, nearestPt);
                    }
                }
            }
            else
            {
                // No player found or too far -> Stop
                if (ai.targetPosition.has_value()) {
                    ai.targetPosition.reset();
                    crowd->resetMoveTarget(ai.crowdAgentIndex);
                }
            }
        }

        ai.timeSinceLastSend += deltaTime;

        if (ai.timeSinceLastSend >= Constants::AI_MESSAGE_RATE)
        {
            ai.timeSinceLastSend = 0.0f;
            const dtCrowdAgent* agent = crowd->getAgent(ai.crowdAgentIndex);

            if (agent)
            {
                float velocityMagnitudeSq = dtVlenSqr(agent->vel);
                if (velocityMagnitudeSq > 0.001f)
                {
                    lobbyMessages[lobby].addUpdate(entity, position.x, position.y, position.z);
                }
            }
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