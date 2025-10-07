#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include <cstdint>

enum class EntityType : uint8_t 
{
    Player = 1,
    AI
};

struct PositionComponent {
    float x;
    float y;
    float z;
};

struct RotationComponent {
    float x;
    float y;
    float z;
};


struct TypeComponent 
{
    EntityType type;
};
