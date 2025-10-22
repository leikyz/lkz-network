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

struct PlayerInputComponent
{
    float inputX;
    float inputY;
    float yaw;
    int sequenceId;
};
struct AIComponent
{
    std::optional<Vector3> targetPosition;
    std::vector<Vector3> path;
    int currentPathIndex = 0;
    float repathTimer = 0.0f;
};
struct PositionComponent
{
    Vector3 position;
};

struct RotationComponent
{
    Vector3 rotation;
};
