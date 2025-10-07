#include "LKZ/Core/ECS/Manager/ComponentManager.h"

void ComponentManager::RemoveEntity(Entity entity)
{
    positions.erase(entity);
    types.erase(entity);
}
