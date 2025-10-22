#include "LKZ/Core/ECS/Manager/ComponentManager.h"
#include "LKZ/Simulation/World.h"
#include "LKZ/Simulation/Math/MathUtils.h"
#include "LKZ/Simulation/Math/Vector.h"
#include <cmath>
#include <LKZ/Core/ECS/System/AISystem.h>
#include <LKZ/Core/Engine.h>
#include "Recast.h"
#include "RecastAlloc.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"
#include <LKZ/Utility/Logger.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Protocol/Message/Entity/MoveEntityMessage.h>
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h" // Inclure le nouveau manager

constexpr float aiMoveSpeed = 0.5f;
constexpr float aiMessageRate = 0.1f; // 10 Hz = every 0.1 seconds
constexpr float aiRepathRate = 1.0f;  // L'IA recalcule le chemin toutes les 1.0 sec

void AISystem::Update(ComponentManager& components, float deltaTime)
{
    // Pour stocker le temps écoulé depuis le dernier envoi pour chaque entité
    static std::unordered_map<Entity, float> timeSinceLastSend;

    // --- RÉCUPÉRATION DU NAVQUERY ---
    World& world = Engine::Instance().GetWorld();
    dtNavMesh* navMesh = world.getNavMesh();
    if (!navMesh) return; // Pas de navmesh, on ne peut rien faire

    // Récupère le query de CE THREAD (le thread "simulation")
    dtNavMeshQuery* navQuery = NavMeshQueryManager::GetThreadLocalQuery(navMesh);
    if (!navQuery) return; // Echec d'allocation ou d'init, on ne peut rien faire
    // --- FIN RÉCUPÉRATION ---


    for (auto& [entity, ai] : components.ai)
    {
        if (!ai.targetPosition.has_value())
            continue;

        Vector3& position = components.positions[entity].position;
        Lobby* lobby = EntityManager::Instance().GetLobbyByEntity(entity);
        if (!lobby) continue;

        // --- LOGIQUE DE REPATHING (CORRECTION DÉSYNCHRO) ---
        ai.repathTimer -= deltaTime;
        bool shouldRepath = false;

        if (ai.path.empty())
        {
            shouldRepath = true;
        }
        else if (ai.repathTimer <= 0.0f)
        {
            shouldRepath = true;
            ai.repathTimer = aiRepathRate; // Réinitialise le timer
        }

        if (shouldRepath)
        {
            // Trouve la position de la cible (joueur 1)
            Entity targetEntity = EntityManager::Instance().GetEntityById(1, lobby);
            if (components.positions.find(targetEntity) == components.positions.end())
            {
                // Le joueur 1 n'existe pas ou n'a pas de position
                ai.path.clear();
                ai.targetPosition.reset();
                continue;
            }
            Vector3& targetPos = components.positions[targetEntity].position;

            // Calcule le chemin en passant le navQuery
            ai.path = world.CalculatePath(
                navQuery,
                position,
                targetPos
            );
            ai.currentPathIndex = 0;
            if (ai.path.empty())
                continue;
        }
        // --- FIN LOGIQUE DE REPATHING ---


        // S'il n'y a toujours pas de chemin (calcul échoué), on arrête
        if (ai.path.empty())
        {
            continue;
        }

        Vector3& target = ai.path[ai.currentPathIndex];

        Vector3 dir = target - position;
        float dist = dir.Length();
        if (dist < 0.05f)
        {
            ai.currentPathIndex++;
            if (ai.currentPathIndex >= ai.path.size())
            {
                ai.path.clear(); // Vide le chemin pour un recalcul à la prochaine frame
                ai.targetPosition.reset(); // Optionnel : si vous voulez que l'IA s'arrête
                continue;
            }
            target = ai.path[ai.currentPathIndex];
            dir = target - position;
            dist = dir.Length();
        }

        dir.Normalize();
        position += dir * aiMoveSpeed * deltaTime;

        float yaw = std::atan2(dir.x, dir.z) * 180.0f / 3.14159265f;
        components.rotations[entity].rotation.y = yaw;

        // Mettre à jour le temps écoulé depuis le dernier envoi
        timeSinceLastSend[entity] += deltaTime;

        // Envoi seulement si 0.1 s écoulées
        if (timeSinceLastSend[entity] >= aiMessageRate)
        {
            timeSinceLastSend[entity] = 0.0f;

            MoveEntityMessage moveMsg(entity, position.x, position.y, position.z);
            Serializer s;
            moveMsg.serialize(s);
            Engine::Instance().Server()->SendToMultiple(
                lobby->clients, s.getBuffer(), moveMsg.getClassName()
            );
        }
    }
}
