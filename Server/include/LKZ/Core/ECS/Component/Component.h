#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include "LKZ/Simulation/Math/Vector.h"
#include "LKZ/Simulation/Math/Quaternion.h"
#include <cstdint>

enum class EntityType : uint8_t 
{
    Player = 1,
    AI
};

struct InputComponent 
{
    Vector2 input;
};

struct PositionComponent 
{
    Vector3 position;
};

struct RotationComponent 
{
    Quaternion rotation;
};

struct TypeComponent 
{
    EntityType type;
};
