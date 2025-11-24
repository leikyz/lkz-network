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

    const float AI_AGGRO_RANGE_SQ = 50.0f * 50.0f;

    const float STOP_DISTANCE = 1.5f;
    const float STOP_DISTANCE_SQ = STOP_DISTANCE * STOP_DISTANCE;

    const int MAX_PATH_UPDATES_PER_TICK = 20;
    int pathUpdatesThisTick = 0;

    for (auto& [entity, ai] : components.ai)
    {
        if (ai.crowdAgentIndex == -1) continue;

        Vector3& position = components.positions[entity].position;
        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        ai.repathTimer -= deltaTime;

        bool needsRepath = (ai.repathTimer <= 0.0f);

        if (needsRepath && pathUpdatesThisTick < MAX_PATH_UPDATES_PER_TICK)
        {
            pathUpdatesThisTick++;

            ai.repathTimer = 0.2f + ((rand() % 10) / 100.0f);

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
                if (pathUpdatesThisTick == 1)
                {
                    
                }

                /*std::string debugMsg = "AI Pos: [" + std::to_string(position.x) + ", " + std::to_string(position.z) + "] " +
                    "| CHASING Player Pos: [" + std::to_string(targetPos.x) + ", " + std::to_string(targetPos.z) + "] " +
                    "| Dist: " + std::to_string(std::sqrt(minDistanceSq));*/
                Logger::Log("AI Y: " + std::to_string(position.y) + " | Player Y: " + std::to_string(targetPos.y), LogType::Debug);
                /*Logger::Log(debugMsg, LogType::Debug);*/
                if (minDistanceSq < STOP_DISTANCE_SQ)
                {
                    ai.targetPosition.reset();
                    crowd->resetMoveTarget(ai.crowdAgentIndex);
                }
                else
                {
                    bool shouldUpdatePath = true;

                    if (ai.targetPosition.has_value())
                    {
                        float distToLastTargetSq = (ai.targetPosition.value() - targetPos).LengthSquared();
    
                        if (distToLastTargetSq < 0.25f)
                        {
                            shouldUpdatePath = false;
                        }
                    }

                    if (shouldUpdatePath)
                    {
                        ai.targetPosition = targetPos; 

                        const float extents[3] = { 5.0f, 5.0f, 5.0f }; 
                        dtPolyRef targetRef;
                        float nearestPt[3];

                        dtStatus status = navQuery->findNearestPoly(targetPos.data(), extents, filter, &targetRef, nearestPt, nullptr);

                        if (dtStatusSucceed(status) && targetRef)
                        {
                            crowd->requestMoveTarget(ai.crowdAgentIndex, targetRef, nearestPt);
                        }
                    }
                }
            }
            else
            {
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