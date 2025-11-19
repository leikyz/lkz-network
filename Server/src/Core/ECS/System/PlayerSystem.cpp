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
#include <LKZ/Core/ECS/System/Player/PlayerSystem.h>
#include <unordered_map>
#include <cmath>
#include <algorithm> // Required for std::sort

void PlayerSystem::Update(ComponentManager& components, float fixedDeltaTime)
{
    static std::unordered_map<Entity, Vector3> lastSentPositions;
    static int tickCounter = 0;
    tickCounter++;

    bool shouldSend = (tickCounter % Constants::PLAYER_MESSAGE_RATE == 0);

    for (auto& [entity, inputComp] : components.playerInputs)
    {
        // Safety Checks & Iterator Access (Faster than map lookup)
        auto posIt = components.positions.find(entity);
        if (posIt == components.positions.end()) continue;

        auto rotIt = components.rotations.find(entity);
        if (rotIt == components.rotations.end()) continue;

        auto stateIt = components.playerState.find(entity);
        if (stateIt == components.playerState.end()) continue;

        auto& positionComponent = posIt->second;
        auto& rotationComponent = rotIt->second;
        auto& playerStateComponent = stateIt->second;

        int lastProcessedSequence = -1;
        bool processedAnyInput = false;

        // --- CRITICAL: Sort inputs by sequence ID to ensure correct order ---
        std::sort(inputComp.inputQueue.begin(), inputComp.inputQueue.end(),
            [](const PlayerInputData& a, const PlayerInputData& b) {
                return a.sequenceId < b.sequenceId;
            });

        // --- PROCESS ALL PENDING INPUTS ---
        for (const auto& input : inputComp.inputQueue)
        {
            // --- CRITICAL FIX: DUPLICATE PROTECTION ---
            // If we already processed this sequence (or a newer one), skip it.
            // This prevents the "double movement" rubber-banding.
            if (input.sequenceId <= inputComp.lastExecutedSequenceId)
                continue;

            // Update trackers
            inputComp.lastExecutedSequenceId = input.sequenceId;
            lastProcessedSequence = input.sequenceId;
            processedAnyInput = true;

            // 1. Calculate Direction
            float yawRad = input.yaw * (Constants::PI / 180.0f);
            float forwardX = std::sin(yawRad);
            float forwardZ = std::cos(yawRad);
            float rightX = std::cos(yawRad);
            float rightZ = -std::sin(yawRad);

            float dirX = rightX * input.inputX + forwardX * input.inputY;
            float dirZ = rightZ * input.inputX + forwardZ * input.inputY;

            float len = std::sqrt(dirX * dirX + dirZ * dirZ);
            if (len > 1.0f)
            {
                dirX /= len;
                dirZ /= len;
            }

            // 2. Calculate Speed
            float speed = Constants::PLAYER_MOVE_SPEED;
            if (playerStateComponent.isArmed)
            {
                if (playerStateComponent.isAiming) speed *= Constants::PLAYER_AIM_SPEED_MULTIPLICATOR;
                else if (playerStateComponent.isRunning) speed *= Constants::PLAYER_RUN_ARMED_SPEED_MULTIPLICATOR;
                else speed *= Constants::PLAYER_WALK_ARMED_SPEED_MULTIPLICATOR;
            }
            else
            {
                if (playerStateComponent.isRunning) speed *= Constants::PLAYER_RUN_SPEED_MULTIPLICATOR;
                else speed *= Constants::PLAYER_WALK_SPEED_MULTIPLICATOR;
            }

            // 3. Apply Movement
            // Use input.deltaTime if sent, otherwise fallback to server FixedDeltaTime
            float dt = (input.deltaTime > 0.0001f) ? input.deltaTime : fixedDeltaTime;

            positionComponent.position.x += dirX * speed * dt;
            positionComponent.position.z += dirZ * speed * dt;

            Logger::Log("Entity " + std::to_string(entity) + " moved to (" +
                std::to_string(positionComponent.position.x) + ", " +
                std::to_string(positionComponent.position.y) + ", " +
                std::to_string(positionComponent.position.z) + ")",
				LogType::Debug);

            // Update Rotation
            rotationComponent.rotation.y = input.yaw;
        }

        // Clear queue after processing
        inputComp.inputQueue.clear();

        // --- NETWORK SYNC ---
        if (!shouldSend && !processedAnyInput) continue;

        Vector3 currentPos = { positionComponent.position.x, positionComponent.position.y, positionComponent.position.z };

        // --- FIX: Initialize lastSentPosition if this is the first time we see this entity ---
        // This prevents the "Snap to Zero" bug when a player first spawns or reconnects.
        if (lastSentPositions.find(entity) == lastSentPositions.end())
        {
            lastSentPositions[entity] = currentPos;
        }

        Vector3 lastPos = lastSentPositions[entity];
        float distSq = MathUtils::Distance(currentPos, lastPos);

        // Only broadcast to others if we moved significantly OR if it's a forced sync tick
        if (distSq < Constants::PLAYER_MOVE_THRESHOLD * Constants::PLAYER_MOVE_THRESHOLD && !shouldSend)
            continue;

        lastSentPositions[entity] = currentPos;

        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        Client* ownerClient = LobbyManager::getClientByEntityId(entity);
        if (!ownerClient) continue;

        // 1. Send Position Update to EVERYONE in the lobby
        MoveEntityMessage moveMsg(entity, positionComponent.position.x, positionComponent.position.y, positionComponent.position.z);
        Serializer s;
        moveMsg.serialize(s);
        Engine::Instance().Server()->SendToMultiple(
            lobby->clients,
            s.getBuffer(),
            moveMsg.getClassName(),
            ownerClient
        );

        // 2. Send Reconciliation/Ack ONLY to the owner
        // We send back the Sequence ID of the LAST input we successfully processed.
        if (lastProcessedSequence != -1)
        {
            LastEntityPositionMessage correctionMsg(
                entity,
                positionComponent.position.x,
                positionComponent.position.y,
                positionComponent.position.z,
                (uint32_t)lastProcessedSequence
            );

            Serializer cs;
            correctionMsg.serialize(cs);

            Engine::Instance().Server()->Send(
                ownerClient->address,
                cs.getBuffer(),
                correctionMsg.getClassName()
            );
        }
    }
}