#pragma once
#include "LKZ/Core/ECS/System/ISystem.h"

class PlayerSystem : public ISystem {
public:
    void Update(ComponentManager& components, float deltaTime) override;
};
