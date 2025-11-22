#pragma once
#include "DetourNavMeshQuery.h"
#include "DetourNavMesh.h"
#include "DetourAlloc.h"
#include <LKZ/Utility/Logger.h> 
#include <LKZ/Simulation/Math/Vector.h>

class NavMeshQueryManager
{
public:
    static dtNavMeshQuery* GetThreadLocalQuery(dtNavMesh* navMesh);
    static void CleanupThreadQuery();
    static Vector3 SnapToNavMesh(dtNavMeshQuery* query, Vector3 approximatePos, float searchRadiusH, float searchRadiusV);
};
