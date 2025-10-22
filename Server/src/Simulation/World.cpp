#include "LKZ/Simulation/World.h"
#include "LKZ/Simulation/Navmesh/NavMeshLoader.h"
#include "LKZ/Utility/Logger.h"
#include <LKZ/Simulation/Math/Vector.h>

#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"

#include <iostream>
#include <format>
#include <random>

#define SAMPLE_POLYAREA_GROUND 1
#define SAMPLE_POLYFLAGS_WALK 0x01

const float DEFAULT_SEARCH_EXTENTS[3] = { 20.0f, 20.0f, 20.0f };


void World::initialize()
{
	Logger::Log("Initializing World...", LogType::Info);
	NavMeshLoader navMeshLoader;

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

	m_filter = new dtQueryFilter();
	m_filter->setIncludeFlags(SAMPLE_POLYFLAGS_WALK);
	m_filter->setExcludeFlags(0);
	m_filter->setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);


	Logger::Log("World and NavMesh initialized successfully.", LogType::Info);
}

void World::update(double deltaTime)
{
}

void World::shutdown()
{
	if (m_filter)
	{
		delete m_filter;
		m_filter = nullptr;
	}
	if (navMesh)
	{
		dtFreeNavMesh(navMesh);
		navMesh = nullptr;
		Logger::Log("NavMesh shut down.", LogType::Info);
	}
	Logger::Log("World shut down.", LogType::Info);
}

Vector3 World::FindNearestPoint(dtNavMeshQuery* navQuery, const Vector3& point)
{
	if (!navQuery || !m_filter) 
	{
		Logger::Log("FindNearestPoint Error: NavQuery or Filter not initialized.", LogType::Error);
		return point;
	}

	dtPolyRef ref;
	float nearestPt[3];
	float startPos[3] = { point.x, point.y, point.z };

	dtStatus status = navQuery->findNearestPoly(startPos, DEFAULT_SEARCH_EXTENTS, m_filter, &ref, nearestPt);

	if (dtStatusFailed(status) || ref == 0)
	{
		Logger::Log(std::format("FindNearestPoint Error: Could not find nearest poly for point ({},{},{}).", point.x, point.y, point.z), LogType::Error);
		return point;
	}

	return { nearestPt[0], nearestPt[1], nearestPt[2] };
}


std::vector<Vector3> World::CalculatePath(dtNavMeshQuery* navQuery, const Vector3& start, const Vector3& end)
{
	std::vector<Vector3> pathPoints;
	if (!navQuery || !m_filter)
	{
		Logger::Log("CalculatePath Error: NavMesh components not initialized.", LogType::Error);
		return pathPoints;
	}

	dtPolyRef startRef, endRef;
	float startPos[3] = { start.x, start.y, start.z };
	float endPos[3] = { end.x, end.y, end.z };
	float nearestStart[3], nearestEnd[3];

	dtStatus status = navQuery->findNearestPoly(startPos, DEFAULT_SEARCH_EXTENTS, m_filter, &startRef, nearestStart);
	if (dtStatusFailed(status) || startRef == 0)
	{
		Logger::Log(std::format("CalculatePath Error: Could not find nearest poly for start point. Status: 0x{:x}, Ref: {}", (unsigned int)status, startRef), LogType::Error);
		return pathPoints;
	}

	status = navQuery->findNearestPoly(endPos, DEFAULT_SEARCH_EXTENTS, m_filter, &endRef, nearestEnd);
	if (dtStatusFailed(status) || endRef == 0)
	{
		Logger::Log(std::format("CalculatePath Error: Could not find nearest poly for end point. Status: 0x{:x}, Ref: {}", (unsigned int)status, endRef), LogType::Error);
		return pathPoints;
	}

	const int MAX_POLYS = 256;
	dtPolyRef polys[MAX_POLYS];
	int npolys;

	status = navQuery->findPath(startRef, endRef, nearestStart, nearestEnd, m_filter, polys, &npolys, MAX_POLYS);
	if (dtStatusFailed(status) || npolys == 0)
	{
		Logger::Log("CalculatePath Error: Could not find path (findPath failed or 0 polys).", LogType::Warning);
		return pathPoints;
	}

	const int MAX_STRAIGHT_PATH = 256;
	float straightPath[MAX_STRAIGHT_PATH * 3];
	unsigned char straightPathFlags[MAX_STRAIGHT_PATH];
	dtPolyRef straightPathPolys[MAX_STRAIGHT_PATH];
	int nstraightPath;

	status = navQuery->findStraightPath(nearestStart, nearestEnd, polys, npolys,
		straightPath, straightPathFlags, straightPathPolys, &nstraightPath, MAX_STRAIGHT_PATH);

	if (dtStatusFailed(status))
	{
		Logger::Log("CalculatePath Error: findStraightPath failed.", LogType::Warning);
		return pathPoints;
	}

	for (int i = 0; i < nstraightPath; i++)
	{
		pathPoints.push_back({ straightPath[i * 3 + 0], straightPath[i * 3 + 1], straightPath[i * 3 + 2] });
	}

	return pathPoints;
}

Vector3 World::getRandomNavMeshPoint(dtNavMeshQuery* navQuery)
{
	if (!navQuery || !m_filter) { 
		return { 0, 0, 0 };
	}

	auto frand = []() -> float {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		return dis(gen);
		};

	dtPolyRef randomRef;
	float randomPt[3];

	dtStatus status = navQuery->findRandomPoint(m_filter, frand, &randomRef, randomPt);

	if (dtStatusSucceed(status))
	{
		return { randomPt[0], randomPt[1], randomPt[2] };
	}

	return { 0, 0, 0 };
}
