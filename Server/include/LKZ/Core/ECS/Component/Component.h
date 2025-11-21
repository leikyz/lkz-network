#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include <LKZ/Simulation/Math/Vector.h>
#include <optional>
#include <vector>
enum class EntitySuperType : uint8_t
{
    Player = 1,
    Zombie = 2,
};

enum class EntityType : uint8_t
{
    Player1 = 1,
    PlayerSynced1 = 2,
    Zombie1 = 3,
};

struct PlayerInputData
{
    float inputX;
    float inputY;
    float yaw;
    int sequenceId;
    float deltaTime;
};

struct PlayerInputComponent
{
    std::vector<PlayerInputData> inputQueue;
    int lastExecutedSequenceId = -1;
};

struct PlayerStateComponent
{
	bool isArmed;
	bool isAiming;
	bool isRunning;
};

struct AIComponent
{
    std::optional<Vector3> targetPosition; 
    float repathTimer;
    int crowdAgentIndex;
    float timeSinceLastSend = 0.0f;
};
struct PositionComponent
{
    Vector3 position;
};

struct RotationComponent
{
    Vector3 rotation;
};
