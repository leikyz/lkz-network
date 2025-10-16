#include "LKZ/Core/ECS/System/Player/MovementSystem.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include <LKZ/Protocol/Message/Entity/RotateEntityMessage.h>
#include <LKZ/Protocol/Message/Entity/LastEntityPositionMessage.h>
#include <LKZ/Simulation/Math/MathUtils.h>
#include <LKZ/Simulation/Math/Vector.h>
#include <LKZ/Utility/Logger.h>
#include <unordered_map>
#include <cmath>

constexpr float moveSpeed = 0.04f;             // Movement speed
constexpr float moveThreshold = 0.02f;         // Minimum movement before sending
constexpr float correctionThreshold = 0.10f;   // Max error before correction
constexpr float sendInterval = 0.1f;           // 10 Hz = every 0.1 s

void MovementSystem::Update(ComponentManager& components, float deltaTime)
{

    static std::unordered_map<Entity, Vector3> lastSentPositions;
    static float sendTimer = 0.0f;
    sendTimer += deltaTime;
    Logger::Log("MovementSystem Update called." + std::to_string(sendTimer), LogType::Debug);
    bool shouldSend = (sendTimer >= sendInterval);
    if (shouldSend)
        sendTimer = 0.0f;

    for (auto& [entity, input] : components.inputs)
    {
        if (components.positions.find(entity) == components.positions.end()) continue;
        if (components.rotations.find(entity) == components.rotations.end()) continue;

        auto& position = components.positions[entity];
        auto& rotation = components.rotations[entity];
        float yawDeg = rotation.y;
        float yawRad = yawDeg * (3.14159265f / 180.0f);

      

        float forwardX = std::sin(yawRad);
        float forwardZ = std::cos(yawRad);
        float rightX = std::cos(yawRad);
        float rightZ = -std::sin(yawRad);

        float dirX = rightX * input.inputX + forwardX * input.inputY;
        float dirZ = rightZ * input.inputX + forwardZ * input.inputY;

        float len = std::sqrt(dirX * dirX + dirZ * dirZ);
        if (len > 0.001f)
        {
            dirX /= len;
            dirZ /= len;
        }

        // --- Apply movement ---
        position.x += dirX * moveSpeed * deltaTime;
        position.z += dirZ * moveSpeed * deltaTime;

        Logger::Log("Entity " + std::to_string(entity) + " moved to (" +
            std::to_string(position.x) + ", " +
            std::to_string(position.y) + ", " +
            std::to_string(position.z) + ")", LogType::Debug);

        // --- Handle network updates (10 Hz) ---
        if (!shouldSend)
            continue;

        Vector3 currentPos = { position.x, position.y, position.z };
        Vector3 lastPos = lastSentPositions[entity];
        float distSq = MathUtils::Distance(currentPos, lastPos);

        if (distSq < moveThreshold * moveThreshold)
            continue; // Skip if entity didn't move enough

        lastSentPositions[entity] = currentPos;

        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby)
            continue;

        Client* ownerClient = LobbyManager::getClientByEntityId(entity);
        if (!ownerClient)
            continue;

        // --- Broadcast movement to other clients ---
        {
            MoveEntityMessage moveMsg(entity, position.x, position.y, position.z);
            Serializer s;
            moveMsg.serialize(s);
            Engine::Instance().Server()->SendToMultiple(
                lobby->clients,
                s.getBuffer(),
                moveMsg.getClassName(),
                ownerClient // exclude owner
            );
        }

        // --- Broadcast rotation to others ---
        {
            RotateEntityMessage rotateMsg(entity, rotation.y);
            Serializer rs;
            rotateMsg.serialize(rs);
            Engine::Instance().Server()->SendToMultiple(
                lobby->clients,
                rs.getBuffer(),
                rotateMsg.getClassName(),
                ownerClient
            );
        }

		// --- Send correction if too far from last known position ---
        uint32_t lastSeq = EntityManager::Instance().GetLastSequenceId(entity);

        LastEntityPositionMessage correctionMsg(
            entity,
            position.x,
            position.y,
            position.z,
            lastSeq
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
