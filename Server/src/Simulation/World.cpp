#include "LKZ/Simulation/World.h"
#include "LKZ/Simulation/Navmesh/NavMeshLoader.h"
#include "LKZ/Utility/Logger.h"
#include <LKZ/Simulation/Math/Vector.h>

// Recast & Detour includes
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"

#include <iostream>
#include <format>

void World::initialize()
{
	Logger::Log("Initializing World...", LogType::Info);
	NavMeshLoader navMeshLoader;

	// Load the mesh data from the file exported by the new Unity script.
	if (!navMeshLoader.LoadFromFile("NavMeshExport.txt"))
	{
		Logger::Log("Critical Error: Could not load navmesh file 'NavMeshExport.txt'.", LogType::Error);
		return;
	}

	Logger::Log("Building NavMesh from loaded data...", LogType::Info);
	navMesh = navMeshLoader.BuildNavMesh();
	if (!navMesh)
	{
		Logger::Log("Critical Error: Failed to build the navmesh from the provided data.", LogType::Error);
		return;
	}

	Logger::Log("World and NavMesh initialized successfully.", LogType::Info);
}

void World::update(double deltaTime)
{
	// Placeholder for future world update logic (e.g., updating agents).
}

void World::shutdown()
{
	if (navMesh)
	{
		dtFreeNavMesh(navMesh);
		navMesh = nullptr;
		Logger::Log("NavMesh shut down and memory freed.", LogType::Info);
	}
}

std::vector<Vector3> World::CalculatePath(const Vector3& start, const Vector3& end)
{
	std::vector<Vector3> pathPoints;

	if (!navMesh)
	{
		Logger::Log("[CalculatePath] Error: navMesh is not initialized. Cannot find a path.", LogType::Error);
		return pathPoints;
	}

	dtNavMeshQuery navQuery;
	navQuery.init(navMesh, 2048);


	dtQueryFilter filter;

	dtPolyRef startRef, endRef;
	float startPos[3] = { start.x, start.y, start.z };
	float endPos[3] = { end.x, end.y, end.z };

	float extents[3] = { 10.0f, 50.0f, 10.0f };

	dtStatus status;

	status = navQuery.findNearestPoly(startPos, extents, &filter, &startRef, nullptr);
	if (dtStatusFailed(status) || startRef == 0)
	{
		Logger::Log("[CalculatePath] Error: findNearestPoly failed for START point. Is it too far from the mesh?", LogType::Warning);
		return pathPoints;
	}

	status = navQuery.findNearestPoly(endPos, extents, &filter, &endRef, nullptr);
	if (dtStatusFailed(status) || endRef == 0)
	{
		Logger::Log("[CalculatePath] Error: findNearestPoly failed for END point. Is it too far from the mesh?", LogType::Warning);
		return pathPoints;
	}

	static const int MAX_POLYS = 256;
	dtPolyRef polys[MAX_POLYS];
	int npolys;

	status = navQuery.findPath(startRef, endRef, startPos, endPos, &filter, polys, &npolys, MAX_POLYS);
	if (dtStatusFailed(status) || npolys == 0)
	{
		Logger::Log("[CalculatePath] Error: findPath could not find a path between the polygons.", LogType::Warning);
		return pathPoints;
	}

	float straightPath[MAX_POLYS * 3];
	unsigned char straightPathFlags[MAX_POLYS];
	dtPolyRef straightPathPolys[MAX_POLYS];
	int nstraightPath;

	status = navQuery.findStraightPath(startPos, endPos, polys, npolys, straightPath, straightPathFlags, straightPathPolys, &nstraightPath, MAX_POLYS);
	if (dtStatusFailed(status))
	{
		Logger::Log("[CalculatePath] Error: findStraightPath failed to smooth the path.", LogType::Warning);
		return pathPoints;
	}

	for (int i = 0; i < nstraightPath; i++)
	{
		Vector3 point;
		point.x = straightPath[i * 3 + 0];
		point.y = straightPath[i * 3 + 1];
		point.z = straightPath[i * 3 + 2];
		pathPoints.push_back(point);
	}

	Logger::Log(std::format("[CalculatePath] Path found with {} points.", nstraightPath), LogType::Debug);
	return pathPoints;
}

