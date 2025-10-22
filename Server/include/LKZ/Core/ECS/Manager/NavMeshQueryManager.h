#pragma once
#include "DetourNavMeshQuery.h"
#include "DetourNavMesh.h"
#include "DetourAlloc.h"
#include <LKZ/Utility/Logger.h> // Assurez-vous d'inclure votre Logger

// Ce manager gère une instance de dtNavMeshQuery par thread.
class NavMeshQueryManager
{
public:
    /**
     * @brief Récupère le dtNavMeshQuery pour le thread courant.
     * L'initialise s'il n'existe pas encore.
     *
     * @param navMesh Le navmesh principal (doit être valide)
     * @return Un pointeur vers le dtNavMeshQuery du thread, ou nullptr en cas d'échec.
     */
    static dtNavMeshQuery* GetThreadLocalQuery(dtNavMesh* navMesh)
    {
        // 'thread_local' signifie que chaque thread aura sa propre copie de ce pointeur.
        static thread_local dtNavMeshQuery* tls_navQuery = nullptr;

        if (!tls_navQuery) // Si ce thread n'a pas encore de query
        {
            tls_navQuery = dtAllocNavMeshQuery();
            if (tls_navQuery)
            {
                // Initialise le query pour ce thread
                if (dtStatusFailed(tls_navQuery->init(navMesh, 2048))) // 2048 = max nodes
                {
                    Logger::Log("NavMeshQueryManager: Échec de l'initialisation du query de thread.", LogType::Error);
                    dtFreeNavMeshQuery(tls_navQuery);
                    tls_navQuery = nullptr;
                }
                else
                {
                    Logger::Log("NavMeshQueryManager: Query initialisé pour ce thread.", LogType::Debug);
                }
            }
            else
            {
                Logger::Log("NavMeshQueryManager: Échec d'allocation du query de thread.", LogType::Error);
            }
        }
        return tls_navQuery;
    }

    /**
     * @brief Nettoie le query pour le thread courant.
     * Doit être appelé avant que le thread ne se termine.
     */
    static void CleanupThreadQuery()
    {
        static thread_local dtNavMeshQuery* tls_navQuery = nullptr;

        if (tls_navQuery)
        {
            dtFreeNavMeshQuery(tls_navQuery);
            tls_navQuery = nullptr;
            Logger::Log("NavMeshQueryManager: Query nettoyé pour ce thread.", LogType::Debug);
        }
    }
};
