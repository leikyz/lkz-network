#include "LKZ/Core/ECS/Manager/ComponentManager.h"

void ComponentManager::RemoveEntity(Entity entity)
{
    positions.erase(entity);
	rotations.erase(entity);
	inputs.erase(entity);
	lastReceivedSequence.erase(entity);
	ai.erase(entity);
}
