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

    // --- CONFIGURATION DE CHASSE ---
    const float AI_AGGRO_RANGE_SQ = 50.0f * 50.0f; // 50 mètres de portée de vue

    // Distance d'arrêt (1.5m)
    const float STOP_DISTANCE = 1.5f;
    const float STOP_DISTANCE_SQ = STOP_DISTANCE * STOP_DISTANCE;

    // Hystérésis : Il faut que le joueur s'éloigne à 2.0m pour recommencer à chasser
    // Cela évite que le zombie vibre s'il est à 1.51m
    const float RESUME_CHASE_DISTANCE = 2.0f;
    const float RESUME_CHASE_DISTANCE_SQ = RESUME_CHASE_DISTANCE * RESUME_CHASE_DISTANCE;

    // Optimisation : Pas plus de 20 calculs de chemin lourds par frame
    const int MAX_PATH_UPDATES_PER_TICK = 20;
    int pathUpdatesThisTick = 0;

    for (auto& [entity, ai] : components.ai)
    {
        if (ai.crowdAgentIndex == -1) continue;

        Vector3& position = components.positions[entity].position;
        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        ai.repathTimer -= deltaTime;

        // On ne tente de recalculer le chemin que si le timer est écoulé
        // ET qu'on a du budget CPU pour cette frame.
        bool needsRepath = (ai.repathTimer <= 0.0f);

        if (needsRepath && pathUpdatesThisTick < MAX_PATH_UPDATES_PER_TICK)
        {
            pathUpdatesThisTick++;

            // Timer aléatoire (0.2s à 0.3s) pour désynchroniser les calculs des zombies
            ai.repathTimer = 0.2f + ((rand() % 10) / 100.0f);

            Entity nearestPlayerEntity = 0;
            float minDistanceSq = FLT_MAX;
            Vector3 targetPos;

            // 1. Trouver le joueur le plus proche
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

            // Chase logic
            if (nearestPlayerEntity != 0 && minDistanceSq < AI_AGGRO_RANGE_SQ)
            {
                // stop
                if (minDistanceSq < STOP_DISTANCE_SQ)
                {
                    if (ai.targetPosition.has_value())
                    {
                        ai.targetPosition.reset();
                        crowd->resetMoveTarget(ai.crowdAgentIndex);

                        // avoid slide
                        dtCrowdAgent* ag = crowd->getEditableAgent(ai.crowdAgentIndex);
                        if (ag) { memset(ag->vel, 0, sizeof(float) * 3); }
                    }
                }
                // chase
                else if (minDistanceSq > RESUME_CHASE_DISTANCE_SQ || ai.targetPosition.has_value())
                {
                    bool shouldUpdatePath = true;

					// If target move is < 25 cm from last target, don't update path
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
				// lost target or too far, stop moving
                if (ai.targetPosition.has_value()) {
                    ai.targetPosition.reset();
                    crowd->resetMoveTarget(ai.crowdAgentIndex);
                }
            }
        }

		// Network update
        ai.timeSinceLastSend += deltaTime;
        if (ai.timeSinceLastSend >= Constants::AI_MESSAGE_RATE)
        {
            ai.timeSinceLastSend = 0.0f;
            const dtCrowdAgent* agent = crowd->getAgent(ai.crowdAgentIndex);

            if (agent)
            {
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