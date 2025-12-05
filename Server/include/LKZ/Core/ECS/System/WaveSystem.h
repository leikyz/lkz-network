#pragma once
#include "LKZ/Core/ECS/System/ISystem.h"

class WaveSystem : public ISystem {
public:
    void Update(ComponentManager& components, float deltaTime) override;
};
