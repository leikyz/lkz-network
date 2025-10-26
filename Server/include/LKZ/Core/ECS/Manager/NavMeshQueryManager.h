#pragma once
#include "DetourNavMeshQuery.h"
#include "DetourNavMesh.h"
#include "DetourAlloc.h"
#include <LKZ/Utility/Logger.h> 

class NavMeshQueryManager
{
public:
    static dtNavMeshQuery* GetThreadLocalQuery(dtNavMesh* navMesh);
    static void CleanupThreadQuery();
    
};
