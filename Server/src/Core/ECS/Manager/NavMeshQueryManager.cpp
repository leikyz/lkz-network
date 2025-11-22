#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h" 

static thread_local dtNavMeshQuery* tls_navQuery = nullptr;

dtNavMeshQuery* NavMeshQueryManager::GetThreadLocalQuery(dtNavMesh* navMesh)
{

    static thread_local dtNavMeshQuery* tls_navQuery = nullptr;

    if (!tls_navQuery)
    {
        tls_navQuery = dtAllocNavMeshQuery();
        if (tls_navQuery)
        {
            if (dtStatusFailed(tls_navQuery->init(navMesh, 2048)))
            {
                Logger::Log("NavMeshQueryManager: Failed to initialize thread-local query.", LogType::Error);
                dtFreeNavMeshQuery(tls_navQuery);
                tls_navQuery = nullptr;
            }
            else
            {
                /*Logger::Log("NavMeshQueryManager: Query initialized for this thread.", LogType::Debug);*/
            }
        }
        else
        {
            /*Logger::Log("NavMeshQueryManager: Failed to allocate thread-local query.", LogType::Error);*/
        }
    }
    return tls_navQuery;
}

void NavMeshQueryManager::CleanupThreadQuery()
{
    static thread_local dtNavMeshQuery* tls_navQuery = nullptr;

    if (tls_navQuery)
    {
        dtFreeNavMeshQuery(tls_navQuery);
        tls_navQuery = nullptr;
        Logger::Log("NavMeshQueryManager: Query cleaned up for this thread.", LogType::Debug);
    }
}
Vector3 NavMeshQueryManager::SnapToNavMesh(dtNavMeshQuery* query, Vector3 approximatePos, float searchRadiusH, float searchRadiusV)
{
    if (!query) return approximatePos;

    const float center[3] = { approximatePos.x, approximatePos.y, approximatePos.z };

    const float extents[3] = { searchRadiusH, searchRadiusV, searchRadiusH };

    dtQueryFilter filter; 
    dtPolyRef polyRef;
    float nearestPt[3];

    dtStatus status = query->findNearestPoly(center, extents, &filter, &polyRef, nearestPt);

    if (dtStatusSucceed(status) && polyRef != 0)
    {
        return Vector3{ nearestPt[0], nearestPt[1], nearestPt[2] };
    }
    return approximatePos;
}