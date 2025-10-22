#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include "LKZ/Core/ECS/Component/Component.h"
#include <unordered_map>

/**
�* @brief Stores all components for entities.
�* Provides access to component data in the ECS architecture.
 */
class ComponentManager
{
public:
	// Singleton instance
	static ComponentManager& Instance()
	{
		static ComponentManager instance;
		return instance;
	}

	// Component storages
	std::unordered_map<Entity, PositionComponent> positions;
	std::unordered_map<Entity, RotationComponent> rotations;
	std::unordered_map<Entity, PlayerInputComponent> inputs;
	std::unordered_map<Entity, AIComponent> ai;
	std::unordered_map<Entity, int> lastReceivedSequence;

	template<typename T>
	void AddComponent(Entity entity, T component);

	template<typename T>
	T* GetComponent(Entity entity);

	void RemoveEntity(Entity entity);

private:
	ComponentManager() = default;
	ComponentManager(const ComponentManager&) = delete;
	ComponentManager& operator=(const ComponentManager&) = delete;
};

