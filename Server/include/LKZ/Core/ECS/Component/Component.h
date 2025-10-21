#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include <LKZ/Simulation/Math/Vector.h>
#include <optional>
#include <vector>
enum class EntityType : uint8_t
{
    Player = 1,
    AI
};

struct PlayerInput 
{
    float inputX;
    float inputY;
    float yaw;
    int sequenceId;
};
struct AIComponent
{
    std::optional<Vector3> targetPosition; // Position que l'IA doit atteindre
    std::vector<Vector3> path;             // Pathfinding result from navmesh
    int currentPathIndex = 0;              // Index du point courant dans le path
};
struct PositionComponent
{
	Vector3 position;
};

struct RotationComponent 
{
	Vector3 rotation;
};


struct TypeComponent
{
    EntityType type;
};