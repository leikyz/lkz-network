#pragma once
#include "LKZ/Core/ECS/Entity.h"
#include <LKZ/Simulation/Math/Vector.h>
#include <optional>
#include <vector>
enum class EntitySuperType : uint8_t
{
    Player = 1,
    Zombie = 2,
};

enum class EntityType : uint8_t
{
    Player1 = 1,
    PlayerSynced1 = 2,
    Zombie1 = 3,
};

struct PlayerInputData
{
    float inputX;
    float inputY;
    float yaw;

    bool isAiming;
    bool isRunning; 
    bool isArmed;   

    int sequenceId;
};

struct PlayerInputComponent
{
    std::vector<PlayerInputData> inputQueue;
    int lastExecutedSequenceId = -1;

    // --- NOUVEAU : État Persistant (Résultat du traitement) ---
    // On stocke ici la vélocité pour l'inertie frame-to-frame
    Vector3 currentVelocity = { 0.0f, 0.0f, 0.0f };

    // On stocke ici le dernier état connu (pour replication / animation / logique de jeu)
    bool isArmed = false;
    bool isAiming = false;
    bool isRunning = false;
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
