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

    const float AI_AGGRO_RANGE_SQ = 200.0f * 200.0f;
    const float STOP_DISTANCE = 1.5f;
    const float STOP_DISTANCE_SQ = STOP_DISTANCE * STOP_DISTANCE;
    const float RESUME_CHASE_DISTANCE = 2.0f;
    const float RESUME_CHASE_DISTANCE_SQ = RESUME_CHASE_DISTANCE * RESUME_CHASE_DISTANCE;
    const float UPDATE_THRESHOLD_SQ = 0.7f * 0.7f;

    const int MAX_PATH_UPDATES_PER_TICK = 50;
    int pathUpdatesThisTick = 0;

    for (auto& [entity, ai] : components.ai)
    {
        if (ai.crowdAgentIndex == -1) continue;

        const dtCrowdAgent* agent = crowd->getAgent(ai.crowdAgentIndex);
        if (!agent) continue;

        Vector3& position = components.positions[entity].position;
        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        ai.repathTimer -= deltaTime;

        bool isIdle = (agent->targetState == DT_CROWDAGENT_TARGET_NONE) ||
            (agent->targetState == DT_CROWDAGENT_TARGET_FAILED);

        bool timerExpired = (ai.repathTimer <= 0.0f);

        bool shouldAttemptPathing = (timerExpired || isIdle) && (pathUpdatesThisTick < MAX_PATH_UPDATES_PER_TICK);

        Entity nearestPlayerEntity = 0;
        float minDistanceSq = FLT_MAX;
        Vector3 targetPos;
        bool foundPlayer = false;

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
                foundPlayer = true;
            }
        }


        if (foundPlayer && minDistanceSq < AI_AGGRO_RANGE_SQ)
        {
            if (minDistanceSq < STOP_DISTANCE_SQ)
            {
                if (ai.targetPosition.has_value() || !isIdle)
                {
                    ai.targetPosition.reset();
                    crowd->resetMoveTarget(ai.crowdAgentIndex);

                    dtCrowdAgent* mutableAgent = crowd->getEditableAgent(ai.crowdAgentIndex);
                    if (mutableAgent) { memset(mutableAgent->vel, 0, sizeof(float) * 3); }
                }
            }
            else if (minDistanceSq > RESUME_CHASE_DISTANCE_SQ || ai.targetPosition.has_value())
            {
                if (shouldAttemptPathing)
                {
                    pathUpdatesThisTick++;
                    ai.repathTimer = 0.2f + ((rand() % 10) / 100.0f); // Reset timer

                    const float extents[3] = { 2.0f, 4.0f, 2.0f };
                    dtPolyRef targetRef;
                    float nearestPt[3];

                    dtStatus status = navQuery->findNearestPoly(targetPos.data(), extents, filter, &targetRef, nearestPt, nullptr);

                    if (dtStatusSucceed(status) && targetRef)
                    {

                        float dx = agent->targetPos[0] - nearestPt[0];
                        float dy = agent->targetPos[1] - nearestPt[1];
                        float dz = agent->targetPos[2] - nearestPt[2];
                        float distToCurrentTargetSq = dx * dx + dy * dy + dz * dz;

                        if (agent->targetState != DT_CROWDAGENT_TARGET_VALID || distToCurrentTargetSq > UPDATE_THRESHOLD_SQ)
                        {
                            ai.targetPosition = targetPos;
                            crowd->requestMoveTarget(ai.crowdAgentIndex, targetRef, nearestPt);
                        }
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

        ai.timeSinceLastSend += deltaTime;
        if (ai.timeSinceLastSend >= Constants::AI_MESSAGE_RATE)
        {
            ai.timeSinceLastSend = 0.0f;

            position.x = agent->npos[0];
            position.y = agent->npos[1];
            position.z = agent->npos[2];

            float velocityMagnitudeSq = dtVlenSqr(agent->vel);

            if (velocityMagnitudeSq > 0.001f)
            {
                float yaw = std::atan2(agent->vel[0], agent->vel[2]) * (180.0f / Constants::PI);

                if (components.rotations.find(entity) != components.rotations.end())
                {
                    components.rotations[entity].rotation.y = yaw;
                }

                lobbyMessages[lobby].addUpdate(entity, position.x, position.y, position.z);
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