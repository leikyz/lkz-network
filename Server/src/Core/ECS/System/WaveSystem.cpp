#include <LKZ/Core/ECS/System/WaveSystem.h>
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Core/Engine.h>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include <LKZ/Protocol/Message/Entity/RotateEntityMessage.h>
#include <LKZ/Protocol/Message/Entity/LastEntityPositionMessage.h>
#include <LKZ/Simulation/Math/MathUtils.h>
#include <LKZ/Simulation/Math/Vector.h>
#include <LKZ/Utility/Logger.h>
#include <LKZ/Utility/Constants.h>
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h"
#include "LKZ/Simulation/World.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include <DetourCrowd.h>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <set>

void WaveSystem::Update(ComponentManager& components, float fixedDeltaTime)
{
    for (auto& [entity, inputComp] : components.waves)
    {
		Logger::Log("Updating WaveComponent for entity: " + std::to_string(entity), LogType::Info);
    }
}