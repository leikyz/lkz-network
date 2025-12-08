#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include <LKZ/Simulation/Math/Vector.h>
#include <optional>
#include <vector>
enum class EntitySuperType : uint8_t
{
    GameManager = 0,
    Player = 1,
    Zombie = 2,
};

enum class EntityType : uint8_t
{
    Player1 = 1,
    PlayerSynced1 = 2,
    ZombieBase1 = 3,
    ZombieBase2 = 4,
	ZombieBase3 = 5
};

struct PlayerInputData
{
    int sequenceId;

    float inputX;
    float inputY;
    float yaw;

    bool isAiming;
    bool isRunning; 
    bool isArmed;   
};

struct PlayerInputComponent
{
    std::vector<PlayerInputData> inputQueue;
    int lastExecutedSequenceId = -1;

    Vector3 currentVelocity = { 0.0f, 0.0f, 0.0f };
};

struct WaveComponent 
{
    int lobbyId;

	bool canSpawn = false;

    int currentWave = 0;
    float stateTimer = 0.0f;
    bool isIntermission = true; 

    int zombiesToSpawn = 0;  
    int zombiesAlive = 0;     
    float spawnTimer = 0.0f;  
};
struct AIComponent
{
    std::optional<Vector3> targetPosition; 
    float repathTimer;
    int crowdAgentIndex;
    float timeSinceLastSend = 0.0f;
};
struct PositionComponent
{
    Vector3 position;
};

struct RotationComponent
{
    Vector3 rotation;
};
