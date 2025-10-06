#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include <cstdint>

enum class EntityType : uint8_t {
    Player = 1,
    AI
};

struct PositionComponent {
    float x;
    float y;
    float z;
};

struct TypeComponent 
{
    EntityType type;
};

struct PlayerComponent {
    // Add player-specific data
    float health;
    float speed;
};

struct AIComponent {
    // Add AI-specific data
    float health;
    float speed;
    int behaviorState;
};
