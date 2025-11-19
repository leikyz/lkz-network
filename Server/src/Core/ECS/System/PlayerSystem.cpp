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
#include <algorithm> 
#include <set>     

void PlayerSystem::Update(ComponentManager& components, float fixedDeltaTime)
{
    static std::unordered_map<Entity, Vector3> lastSentPositions;
    static int tickCounter = 0;
    tickCounter++;

    // Debug set to log initialization only once per entity
    static std::set<Entity> debuggedEntities;

    bool shouldSend = (tickCounter % Constants::PLAYER_MESSAGE_RATE == 0);

    for (auto& [entity, inputComp] : components.playerInputs)
    {
        // Safety Checks & Iterator Access
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

        // --- DEBUG: LOG INITIAL STATE ONCE ---
        if (debuggedEntities.find(entity) == debuggedEntities.end())
        {
            debuggedEntities.insert(entity);
           /* Logger::Log("[INIT] Entity " + std::to_string(entity) +
                " QueueSize=" + std::to_string(inputComp.inputQueue.size()) +
                " LastExec=" + std::to_string(inputComp.lastExecutedSequenceId),
                LogType::Info);*/
        }

        // Sort inputs to process in order
        std::sort(inputComp.inputQueue.begin(), inputComp.inputQueue.end(),
            [](const PlayerInputData& a, const PlayerInputData& b) {
                return a.sequenceId < b.sequenceId;
            });

        // --- PROCESS ALL PENDING INPUTS ---
        for (const auto& input : inputComp.inputQueue)
        {
            if (input.sequenceId <= inputComp.lastExecutedSequenceId)
            {
               /* Logger::Log("[REJECT] Entity " + std::to_string(entity) +
                    " Input Seq " + std::to_string(input.sequenceId) +
                    " <= LastExec " + std::to_string(inputComp.lastExecutedSequenceId),
                    LogType::Warning);*/
                continue;
            }

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
            float dt = (input.deltaTime > 0.0001f) ? input.deltaTime : fixedDeltaTime;

            // --- DEBUG MOVEMENT DETAILS (Throttle to avoid spam, but log first few) ---
            if (processedAnyInput && tickCounter % 10 == 0)
            {
                /*Logger::Log("[MOVE DETAIL] Entity " + std::to_string(entity) +
                    " Speed=" + std::to_string(speed) +
                    " DT=" + std::to_string(dt) +
                    " InX=" + std::to_string(input.inputX) +
                    " InY=" + std::to_string(input.inputY),
                    LogType::Debug);*/
            }

            // Safety warning for Zero DT
            if (dt <= 0.00001f) {
                Logger::Log("[ERROR] DT is ZERO for Entity " + std::to_string(entity), LogType::Error);
            }

            positionComponent.position.x += dirX * speed * dt;
            positionComponent.position.z += dirZ * speed * dt;

            Logger::Log("[POSITION UPDATE] Entity " + std::to_string(entity) +
                " NewPos=(" + std::to_string(positionComponent.position.x) + ", " +
                std::to_string(positionComponent.position.y) + ", " +
                std::to_string(positionComponent.position.z) + ")",
				LogType::Debug);

            // Update Rotation
            rotationComponent.rotation.y = input.yaw;
        }

        // Log queue size if it's growing too large (lag backlog)
        if (inputComp.inputQueue.size() > 10) {
          /*  Logger::Log("Entity " + std::to_string(entity) + " Queue BACKLOG: " + std::to_string(inputComp.inputQueue.size()), LogType::Warning);*/
        }

        // Clear queue after processing
        inputComp.inputQueue.clear();

        // --- NETWORK SYNC ---
        if (!shouldSend && !processedAnyInput) continue;

        Vector3 currentPos = { positionComponent.position.x, positionComponent.position.y, positionComponent.position.z };

        // --- INITIALIZATION FIX ---
        if (lastSentPositions.find(entity) == lastSentPositions.end())
        {
            lastSentPositions[entity] = currentPos;
            Logger::Log("Entity " + std::to_string(entity) + " Initialized LastPos to current.", LogType::Info);
        }

        Vector3 lastPos = lastSentPositions[entity];
        float distSq = MathUtils::Distance(currentPos, lastPos);

        if (distSq < Constants::PLAYER_MOVE_THRESHOLD * Constants::PLAYER_MOVE_THRESHOLD && !shouldSend)
            continue;

        lastSentPositions[entity] = currentPos;

        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        Client* ownerClient = LobbyManager::getClientByEntityId(entity);
        if (!ownerClient) continue;

        // 1. Send Position Update to EVERYONE
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