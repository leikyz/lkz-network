#pragma once
#include "LKZ/Core/ECS/System/ISystem.h"

class WaveSystem : public ISystem {
public:
    void Update(ComponentManager& components, float deltaTime) override;
private:
    void SpawnZombie(int lobbyId, int entitySuperTypeId);

    const int MAX_ZOMBIES_ON_MAP = 24; 
};
