#include "LKZ/Core/ECS/Manager/ComponentManager.h"

void ComponentManager::RemoveEntity(Entity entity)
{

	positions.erase(entity);
	rotations.erase(entity);
	inputs.erase(entity);
	lastReceivedSequence.erase(entity);
	ai.erase(entity);
}


template<>
void ComponentManager::AddComponent<PositionComponent>(Entity entity, PositionComponent component)
{
	positions[entity] = component;
}

template<>
void ComponentManager::AddComponent<RotationComponent>(Entity entity, RotationComponent component)
{
	rotations[entity] = component;
}

template<>
void ComponentManager::AddComponent<PlayerInputComponent>(Entity entity, PlayerInputComponent component)
{
	inputs[entity] = component;
}

template<>
void ComponentManager::AddComponent<AIComponent>(Entity entity, AIComponent component)
{
	ai[entity] = component;
}


template<>
PositionComponent* ComponentManager::GetComponent<PositionComponent>(Entity entity)
{
	auto it = positions.find(entity);
	return (it != positions.end()) ? &it->second : nullptr;
}


