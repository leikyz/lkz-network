#include "LKZ/Core/ECS/Manager/SystemManager.h"

void SystemManager::RegisterSystem(std::shared_ptr<ISystem> system)
{
    systems.push_back(system);
}

void SystemManager::Update(ComponentManager& components, float deltaTime)
{
    for (auto& system : systems)
    {
        system->Update(components, deltaTime);
    }
}
