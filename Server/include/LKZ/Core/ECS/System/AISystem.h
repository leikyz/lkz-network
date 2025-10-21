#pragma once
#include "LKZ/Core/ECS/System/ISystem.h"

class AISystem : public ISystem {
public:
    void Update(ComponentManager& components, float deltaTime) override;
};
