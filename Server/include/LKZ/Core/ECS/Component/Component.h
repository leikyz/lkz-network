#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include <cstdint>

enum class EntityType : uint8_t 
{
    Player = 1,
    AI
};

struct InputComponent 
{
    float inputX = 0.0f;
    float inputY = 0.0f;
};

struct MouseInputComponent
{
    float inputX = 0.0f;
    float inputY = 0.0f;
};

struct PositionComponent 
{
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
