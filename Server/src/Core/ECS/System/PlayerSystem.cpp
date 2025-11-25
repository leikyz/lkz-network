#include <LKZ/Core/ECS/System/Player/PlayerSystem.h>
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Core/Engine.h>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include <LKZ/Protocol/Message/Entity/RotateEntityMessage.h>
#include <LKZ/Protocol/Message/Entity/LastEntityPositionMessage.h>
#include <LKZ/Simulation/Math/MathUtils.h>
#include <LKZ/Simulation/Math/Vector.h>
#include <LKZ/Utility/Logger.h>
#include <LKZ/Utility/Constants.h>
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h"
#include "LKZ/Simulation/World.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
// ---------------------------
#include <DetourCrowd.h>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <set>

constexpr float INERTIA_DAMPING = 8.0f;

void UpdateVelocity(Vector3& velocity, const PlayerInputData& input, float speed, float fixedDeltaTime)
{
    float yawRad = input.yaw * (Constants::PI / 180.0f);

    Vector3 forwardDir = { std::sin(yawRad), 0, std::cos(yawRad) };
    Vector3 rightDir = { std::cos(yawRad), 0, -std::sin(yawRad) };

    Vector3 targetDir = {
        (rightDir.x * input.inputX) + (forwardDir.x * input.inputY),
        0.0f,
        (rightDir.z * input.inputX) + (forwardDir.z * input.inputY)
    };

    float len = std::sqrt(targetDir.x * targetDir.x + targetDir.z * targetDir.z);
    if (len > 1.0f) {
        targetDir.x /= len;
        targetDir.z /= len;
    }

    Vector3 targetVel = { targetDir.x * speed, 0.0f, targetDir.z * speed };

    float blend = INERTIA_DAMPING * fixedDeltaTime;
    if (blend > 1.0f) blend = 1.0f;

    velocity.x += (targetVel.x - velocity.x) * blend;
    velocity.z += (targetVel.z - velocity.z) * blend;

    if (std::abs(velocity.x) < 0.01f && input.inputX == 0 && input.inputY == 0) velocity.x = 0;
    if (std::abs(velocity.z) < 0.01f && input.inputX == 0 && input.inputY == 0) velocity.z = 0;
}

void PlayerSystem::Update(ComponentManager& components, float fixedDeltaTime)
{
    World& world = Engine::Instance().GetWorld();
    dtNavMeshQuery* navQuery = NavMeshQueryManager::GetThreadLocalQuery(world.getNavMesh());
    const dtQueryFilter* filter = world.getCrowd() ? world.getCrowd()->getFilter(0) : nullptr;

    static int tickCounter = 0;
    tickCounter++;

    bool shouldSend = (tickCounter % Constants::PLAYER_MESSAGE_RATE == 0);

    const float extents[3] = { 2.0f, 4.0f, 2.0f };

    for (auto& [entity, inputComp] : components.playerInputs)
    {
        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        Client* ownerClient = LobbyManager::getClientByEntityId(entity);
        if (!ownerClient) continue;

        auto& pos = components.positions[entity].position;
        auto& vel = components.playerState[entity].currentVelocity;
        auto state = components.playerState[entity];

        std::sort(inputComp.inputQueue.begin(), inputComp.inputQueue.end(),
            [](const PlayerInputData& a, const PlayerInputData& b) {
                return a.sequenceId < b.sequenceId;
            });

        int lastProcessedSeq = -1;
        bool hasProcessed = false;

		float speed = Constants::PLAYER_MOVE_SPEED;

        if (state.isAiming)
			speed *= Constants::PLAYER_AIM_SPEED_MULTIPLICATOR;
        else if (state.isArmed)
        {
            if (state.isRunning)
                speed *= Constants::PLAYER_RUN_ARMED_SPEED_MULTIPLICATOR;
            else
                speed *= Constants::PLAYER_WALK_ARMED_SPEED_MULTIPLICATOR;
        }
        else
        {
            if (state.isRunning)
                speed *= Constants::PLAYER_RUN_SPEED_MULTIPLICATOR;
            else
                speed *= Constants::PLAYER_WALK_SPEED_MULTIPLICATOR;
        }

        dtPolyRef currentPolyRef = 0;
        float startPos[3] = { pos.x, pos.y, pos.z };

        if (navQuery && filter)
        {
            navQuery->findNearestPoly(startPos, extents, filter, &currentPolyRef, startPos);
            if (currentPolyRef != 0)
            {
                pos.y = startPos[1];
            }
            else
            {
               /* Logger::Log(std::format("[NavError] Entity {} is OFF MESH at ({:.2f}, {:.2f}, {:.2f}). Extents too small or World Flipped?",
                    entity, pos.x, pos.y, pos.z), LogType::Warning);*/
            }
        }

        for (const auto& input : inputComp.inputQueue)
        {
            if (input.sequenceId <= inputComp.lastExecutedSequenceId) continue;

            UpdateVelocity(vel, input, speed, fixedDeltaTime);

            if (navQuery && filter && currentPolyRef != 0)
            {
                float endPos[3];
                endPos[0] = startPos[0] + (vel.x * fixedDeltaTime);
                endPos[1] = startPos[1];
                endPos[2] = startPos[2] + (vel.z * fixedDeltaTime);

                float resultPos[3];
                dtPolyRef visitedPolys[16];
                int nVisited = 0;

                navQuery->moveAlongSurface(
                    currentPolyRef,
                    startPos,
                    endPos,
                    filter,
                    resultPos,
                    visitedPolys,
                    &nVisited,
                    16
                );

                pos.x = resultPos[0];
                pos.y = resultPos[1];
                pos.z = resultPos[2];

                startPos[0] = resultPos[0];
                startPos[1] = resultPos[1];
                startPos[2] = resultPos[2];

                if (nVisited > 0)
                {
                    currentPolyRef = visitedPolys[nVisited - 1];
                }
            }
            else
            {
                pos.x += vel.x * fixedDeltaTime;
                pos.z += vel.z * fixedDeltaTime;
            }

          /*  Logger::Log(std::format("Player {} Input Seq {}: Pos({:.2f}, {:.2f}, {:.2f}) Vel({:.2f}, {:.2f}, {:.2f})",
                entity,
                input.sequenceId,
                pos.x, pos.y, pos.z,
				vel.x, vel.y, vel.z), LogType::Debug);*/

            inputComp.lastExecutedSequenceId = input.sequenceId;
            lastProcessedSeq = input.sequenceId;
            hasProcessed = true;
        }

        inputComp.inputQueue.clear();

        if (/*shouldSend &&*/ hasProcessed)
        {
            MoveEntityMessage moveMsg(entity, pos.x, pos.y, pos.z);
            Serializer s;
            moveMsg.serialize(s);
            Engine::Instance().Server()->SendToMultiple(
                lobby->clients,
                s.getBuffer(),
                moveMsg.getClassName(),
                ownerClient
            );
        }

        if (hasProcessed)
        {
            Serializer s;
            LastEntityPositionMessage msg(
                entity,
                pos.x, pos.y, pos.z,
                vel.x, vel.y, vel.z,
                lastProcessedSeq
            );

            msg.serialize(s);
            Engine::Instance().Server()->Send(ownerClient->address, s.getBuffer(), msg.getClassName());
        }
    }
}