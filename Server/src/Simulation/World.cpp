#include "LKZ/Simulation/World.h"
#include "LKZ/Simulation/Navmesh/NavMeshLoader.h"
#include "LKZ/Utility/Logger.h"
#include <LKZ/Simulation/Math/Vector.h>

#include "Recast.h"
#include "RecastAlloc.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"

#include <iostream>


    void World::initialize()
    {
        Logger::Log("Initializing World...", LogType::Info);
        NavMeshLoader navMeshLoader;
        if (!navMeshLoader.LoadFromFile("NavMeshExport.txt"))
        {
            std::cerr << "Error: Can't load navmesh file" << std::endl;
            return;
        }

        Logger::Log("Loading world...", LogType::Info);
        navMesh = navMeshLoader.BuildNavMesh();
        if (!navMesh)
        {
            std::cerr << "Error: Can't build the navmesh" << std::endl;
            return;
        }

        Logger::Log("World loaded!", LogType::Info);
    }

    std::vector<Vector3> World::CalculatePath(const Vector3& start, const Vector3& end)
    {
        std::vector<Vector3> pathPoints;

        if (!navMesh)
            return pathPoints;

        dtNavMeshQuery navQuery;
        navQuery.init(navMesh, 2048);

        dtQueryFilter filter;
        dtPolyRef startRef, endRef;
        float startPos[3] = { start.x, start.y, start.z };
        float endPos[3] = { end.x, end.y, end.z };

        float extents[3] = { 2.0f, 4.0f, 2.0f };

        if (dtStatusFailed(navQuery.findNearestPoly(startPos, extents, &filter, &startRef, nullptr))) return pathPoints;
        if (dtStatusFailed(navQuery.findNearestPoly(endPos, extents, &filter, &endRef, nullptr))) return pathPoints;

        static const int MAX_POLYS = 256;
        dtPolyRef polys[MAX_POLYS];
        int npolys;
        float straightPath[MAX_POLYS * 3];
        unsigned char straightPathFlags[MAX_POLYS];
        dtPolyRef straightPathPolys[MAX_POLYS];
        int nstraightPath;

        if (dtStatusFailed(navQuery.findPath(startRef, endRef, startPos, endPos, &filter, polys, &npolys, MAX_POLYS)))
            return pathPoints;

        if (dtStatusFailed(navQuery.findStraightPath(startPos, endPos, polys, npolys, straightPath, straightPathFlags, straightPathPolys, &nstraightPath, MAX_POLYS)))
            return pathPoints;

        for (int i = 0; i < nstraightPath; i++)
        {
            Vector3 point;
            point.x = straightPath[i * 3 + 0];
            point.y = straightPath[i * 3 + 1];
            point.z = straightPath[i * 3 + 2];
            pathPoints.push_back(point);
        }

        return pathPoints;
    }

