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

constexpr float INERTIA_DAMPING = 8.0f;
void ApplyPhysics(Vector3& position, Vector3& velocity, const PlayerInputData& input, float fixedDeltaTime)
{
    float yawRad = input.yaw * (3.14159265359f / 180.0f);

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

    Vector3 targetVel = { targetDir.x * Constants::PLAYER_MOVE_SPEED, 0.0f, targetDir.z * Constants::PLAYER_MOVE_SPEED };

    float blend = INERTIA_DAMPING * fixedDeltaTime;
    if (blend > 1.0f) blend = 1.0f;

    velocity.x += (targetVel.x - velocity.x) * blend;
    velocity.z += (targetVel.z - velocity.z) * blend;

    if (std::abs(velocity.x) < 0.01f && input.inputX == 0 && input.inputY == 0) velocity.x = 0;
    if (std::abs(velocity.z) < 0.01f && input.inputX == 0 && input.inputY == 0) velocity.z = 0;

    position.x += velocity.x * fixedDeltaTime;
    position.z += velocity.z * fixedDeltaTime;
}
void PlayerSystem::Update(ComponentManager& components, float fixedDeltaTime)
{
    for (auto& [entity, inputComp] : components.playerInputs)
    {

        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        Client* ownerClient = LobbyManager::getClientByEntityId(entity);
        if (!ownerClient) continue;

        auto& pos = components.positions[entity].position;
        auto& vel = components.playerState[entity].currentVelocity; 

        std::sort(inputComp.inputQueue.begin(), inputComp.inputQueue.end(),
            [](const PlayerInputData& a, const PlayerInputData& b) {
                return a.sequenceId < b.sequenceId;
            });

        int lastProcessedSeq = -1;
        bool hasProcessed = false;

        for (const auto& input : inputComp.inputQueue)
        {
            if (input.sequenceId <= inputComp.lastExecutedSequenceId) continue;

            ApplyPhysics(pos, vel, input, fixedDeltaTime);

            inputComp.lastExecutedSequenceId = input.sequenceId;
            lastProcessedSeq = input.sequenceId;
            hasProcessed = true;
        }

        inputComp.inputQueue.clear();

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