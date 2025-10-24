#include "LKZ/Core/ECS/System/Player/PlayerSystem.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Core/Engine.h>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include <LKZ/Protocol/Message/Entity/RotateEntityMessage.h>
#include <LKZ/Protocol/Message/Entity/LastEntityPositionMessage.h>
#include <LKZ/Simulation/Math/MathUtils.h>
#include <LKZ/Simulation/Math/Vector.h>
#include <LKZ/Utility/Logger.h>
#include <unordered_map>
#include <cmath>

constexpr float moveSpeed = 0.2f; // 0.2f
constexpr int sendEveryTicks = 5;     
constexpr float moveThreshold = 0.02f; 

void PlayerSystem::Update(ComponentManager& components, float fixedDeltaTime)
{
    static std::unordered_map<Entity, Vector3> lastSentPositions;
    static int tickCounter = 0;
    tickCounter++;

    bool shouldSend = (tickCounter % sendEveryTicks == 0);

    for (auto& [entity, input] : components.inputs)
    {
        if (components.positions.find(entity) == components.positions.end()) continue;
        if (components.rotations.find(entity) == components.rotations.end()) continue;


        auto& positionComponent = components.positions[entity];
        auto& rotationComponent = components.rotations[entity];

        float yawRad = rotationComponent.rotation.y * (3.14159265f / 180.0f);
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

        positionComponent.position.x += dirX * moveSpeed * fixedDeltaTime;
        positionComponent.position.z += dirZ * moveSpeed * fixedDeltaTime;

        /* Logger::Log(
             std::format("[Server] Entity {} pos: x={:.3f}, y={:.3f}, z={:.3f}, dt={:.3f}",
                 entity, positionComponent.position.x, positionComponent.position.y, positionComponent.position.z, fixedDeltaTime),
             LogType::Debug
         );*/

        if (!shouldSend)
            continue;

        Vector3 currentPos = { positionComponent.position.x, positionComponent.position.y, positionComponent.position.z };
        Vector3 lastPos = lastSentPositions[entity];
        float distSq = MathUtils::Distance(currentPos, lastPos);

        if (distSq < moveThreshold * moveThreshold)
            continue;

        lastSentPositions[entity] = currentPos;

        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby)
            continue;

        Client* ownerClient = LobbyManager::getClientByEntityId(entity);
        if (!ownerClient)
            continue;

        MoveEntityMessage moveMsg(entity, positionComponent.position.x, positionComponent.position.y, positionComponent.position.z);
        Serializer s;
        moveMsg.serialize(s);
        Engine::Instance().Server()->SendToMultiple(
            lobby->clients,
            s.getBuffer(),
            moveMsg.getClassName(),
            ownerClient
        );

        RotateEntityMessage rotateMsg(entity, rotationComponent.rotation.y);
        Serializer rs;
        rotateMsg.serialize(rs);
        Engine::Instance().Server()->SendToMultiple(
            lobby->clients,
            rs.getBuffer(),
            rotateMsg.getClassName(),
            ownerClient
        );


        uint32_t lastSeq = EntityManager::Instance().GetLastSequenceId(entity);

        LastEntityPositionMessage correctionMsg(
            entity,
            positionComponent.position.x,
            positionComponent.position.y,
            positionComponent.position.z,
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
