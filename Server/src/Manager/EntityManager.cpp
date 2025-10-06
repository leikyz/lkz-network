#include "LKZ/Core/Manager/EntityManager.h"

Entity EntityManager::CreateEntity() {
    Entity id;
    if (!freeIDs.empty()) {
        id = freeIDs.front();
        freeIDs.pop();
    }
    else {
        id = nextID++;
    }
    return id;
}

void EntityManager::DestroyEntity(Entity entity) {
    freeIDs.push(entity);
}
