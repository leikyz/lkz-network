#include "LKZ/Core/ECS/System/Player/MovementSystem.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Utility/Logger.h>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Simulation/Math/MathUtils.h>
#include <LKZ/Simulation/Math/Vector.h>
#include <unordered_map>
#include <cmath>

constexpr float moveSpeed = 0.04f;  // Movement speed
constexpr float moveThreshold = 0.02f;  // Minimum movement before sending (m)
constexpr float correctionThreshold = 0.10f; // Max error before correction (m)
constexpr float sendInterval = 0.1f;   // 10 Hz = every 0.1 s

void MovementSystem::Update(ComponentManager& components, float deltaTime)
{
    static std::unordered_map<Entity, Vector3> lastSentPositions;
    static float sendTimer = 0.0f;
    sendTimer += deltaTime;

    // ---- Simulation tick (every frame) ----
    for (auto& [entity, position] : components.positions)
    {
        Vector3 currentPos = { position.x, position.y, position.z };
        Vector3 lastPos = lastSentPositions[entity];
        float distSq = MathUtils::Distance(currentPos, lastPos);

        if (distSq > moveThreshold * moveThreshold)
        {
            lastSentPositions[entity] = currentPos;

            Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
            if (!lobby) continue;

            Client* entityClient = LobbyManager::getClientByEntityId(entity);
            if (!entityClient) continue;

            // --- Broadcast to other players ---
            MoveEntityMessage moveMsg(entity, position.x, position.y, position.z);
            Serializer s;
            moveMsg.serialize(s);

            Engine::Instance().Server()->SendToMultiple(
                lobby->clients,
                s.getBuffer(),
                moveMsg.getClassName(),
                entityClient
            );

            // --- Check for owner correction ---
            //Vector3 clientReportedPos = entityClient->GetClientReportedPosition(entity);
            //float errorSq = MathUtils::Distance(currentPos, clientReportedPos);

            //if (errorSq > correctionThreshold * correctionThreshold)
            //{
            //    // Send correction message to owner
            //    MoveEntityMessage correctionMsg(entity, position.x, position.y, position.z);
            //    Serializer corrS;
            //    correctionMsg.serialize(corrS);
            //    Engine::Instance().Server()->Send(entityClient->address, corrS.getBuffer(), correctionMsg.getClassName());
            //}
        }
    }

}
