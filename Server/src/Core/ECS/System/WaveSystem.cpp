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
#include <LKZ/Protocol/Message/Gameplay/ChangeWaveMessage.h>

void WaveSystem::Update(ComponentManager& components, float fixedDeltaTime)
{
    for (auto& [entity, waveComponent] : components.waves)
    {

		Lobby* lobby = LobbyManager::getLobby(waveComponent.lobbyId);

		// Check if lobby is valid and in game
		if (!lobby || !lobby->inGame)
			continue;

		Serializer serializer;

		// Start timer update
		if (waveComponent.spawnTimer < 5.0f)
		{
			waveComponent.spawnTimer += fixedDeltaTime;
		}
		else if (waveComponent.isIntermission)
		{		
			waveComponent.currentWave++;
			ChangeWaveMessage changeWaveMsg(waveComponent.currentWave);
			changeWaveMsg.serialize(serializer);

			Engine::Instance().Server()->SendToMultiple(
				lobby->clients,
				serializer.getBuffer(),
				changeWaveMsg.getClassName());

			waveComponent.isIntermission = false;
		}

		
		
		Logger::Log("Updating WaveComponent for entity: " + std::to_string(waveComponent.spawnTimer), LogType::Info);
    }
}