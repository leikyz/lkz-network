#pragma once
#include <vector>
#include <memory>
#include "LKZ/Core/ECS/System/ISystem.h"
#include "LKZ/Core/Manager/ComponentManager.h"

class SystemManager
{
public:
    void RegisterSystem(std::shared_ptr<ISystem> system);
    void Update(ComponentManager& components, float deltaTime);

private:
    std::vector<std::shared_ptr<ISystem>> systems;
};
