#pragma once
#include "LKZ/Core/Manager/ComponentManager.h"

class ISystem {
public:
    virtual ~ISystem() = default;

    // Pure virtual update method every system must implement
    virtual void Update(ComponentManager& components, float deltaTime) = 0;
};
