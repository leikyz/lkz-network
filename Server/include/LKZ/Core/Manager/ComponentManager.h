#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include "LKZ/Core/ECS/Component/Component.h"
#include <unordered_map>

class ComponentManager {
public:
    std::unordered_map<Entity, PositionComponent> positions;
    std::unordered_map<Entity, TypeComponent> types;
    std::unordered_map<Entity, PlayerComponent> players;
    std::unordered_map<Entity, AIComponent> ais;

    void RemoveEntity(Entity entity);
};
