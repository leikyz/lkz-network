#pragma once
#include <vector>
#include <memory>
#include "LKZ/Core/ECS/System/ISystem.h"
#include "LKZ/Core/ECS/Manager/ComponentManager.h"

class SystemManager
{
public:
    // Singleton instance access
    static SystemManager& Instance()
    {
        static SystemManager instance;
        return instance;
    }

    void RegisterSystem(std::shared_ptr<ISystem> system);
    void Update(ComponentManager& components, float deltaTime);

private:
    std::vector<std::shared_ptr<ISystem>> systems;

    // Private constructor to enforce singleton
    SystemManager() = default;
    SystemManager(const SystemManager&) = delete;
    SystemManager& operator=(const SystemManager&) = delete;
};
