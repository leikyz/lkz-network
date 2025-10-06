#include "LKZ/Core/Manager/ComponentManager.h"

void ComponentManager::RemoveEntity(Entity entity) 
{
    positions.erase(entity);
    types.erase(entity);
    players.erase(entity);
    ais.erase(entity);
}
