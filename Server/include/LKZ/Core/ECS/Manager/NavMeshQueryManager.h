#pragma once
#include "DetourNavMeshQuery.h"
#include "DetourNavMesh.h"
#include "DetourAlloc.h"
#include <LKZ/Utility/Logger.h> 
class NavMeshQueryManager
{
public:
    static dtNavMeshQuery* GetThreadLocalQuery(dtNavMesh* navMesh)
    {
        static thread_local dtNavMeshQuery* tls_navQuery = nullptr;

        if (!tls_navQuery) 
        {
            tls_navQuery = dtAllocNavMeshQuery();
            if (tls_navQuery)
            {
                if (dtStatusFailed(tls_navQuery->init(navMesh, 2048)))
                {
                    Logger::Log("NavMeshQueryManager: �chec de l'initialisation du query de thread.", LogType::Error);
                    dtFreeNavMeshQuery(tls_navQuery);
                    tls_navQuery = nullptr;
                }
                else
                {
                    Logger::Log("NavMeshQueryManager: Query initialis� pour ce thread.", LogType::Debug);
                }
            }
            else
            {
                Logger::Log("NavMeshQueryManager: �chec d'allocation du query de thread.", LogType::Error);
            }
        }
        return tls_navQuery;
    }

    static void CleanupThreadQuery()
    {
        static thread_local dtNavMeshQuery* tls_navQuery = nullptr;

        if (tls_navQuery)
        {
            dtFreeNavMeshQuery(tls_navQuery);
            tls_navQuery = nullptr;
            Logger::Log("NavMeshQueryManager: Query nettoy� pour ce thread.", LogType::Debug);
        }
    }
};
