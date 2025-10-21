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

// Taille de la boîte de recherche pour "snapper" au navmesh
// {x, y, z} (demi-taille)
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

	// --- INITIALISATION DES MEMBRES ---
	m_navQuery = dtAllocNavMeshQuery();
	if (m_navQuery)
	{
		m_navQuery->init(navMesh, 2048); // 2048 = max nodes
	}

	m_filter = new dtQueryFilter();
	m_filter->setIncludeFlags(SAMPLE_POLYFLAGS_WALK);
	m_filter->setExcludeFlags(0);
	m_filter->setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
	// --- FIN INITIALISATION ---


	Logger::Log("World and NavMesh initialized successfully.", LogType::Info);

	// --- DÉBUT DE LA SIMULATION DE PATHFINDING ---
	// Ce test utilise maintenant les membres initialisés
	Logger::Log("--- Running Pathfinding Test ---", LogType::Info);

	// Coordonnées valides de NavMeshExport.txt
	Vector3 startPoint = { -679.6347f, 267.031067f, 1252.51611f };
	Vector3 endPoint = { -685.6347f, 281.197754f, 1241.18274f };

	Logger::Log(std::format("Calculating path from A: ({}, {}, {})", startPoint.x, startPoint.y, startPoint.z), LogType::Info);
	Logger::Log(std::format("To B: ({}, {}, {})", endPoint.x, endPoint.y, endPoint.z), LogType::Info);

	std::vector<Vector3> path = CalculatePath(startPoint, endPoint);

	if (path.empty())
	{
		Logger::Log("Pathfinding Test Result: NO PATH FOUND.", LogType::Warning);
	}
	else
	{
		Logger::Log(std::format("Pathfinding Test Result: Path found with {} points.", path.size()), LogType::Info);
		for (size_t i = 0; i < path.size(); ++i)
		{
			const auto& point = path[i];
			Logger::Log(std::format("  Point {}: ({}, {}, {})", i, point.x, point.y, point.z), LogType::Info);
		}
	}
	Logger::Log("--- Pathfinding Test Complete ---", LogType::Info);
	// --- FIN DE LA SIMULATION ---
}

void World::update(double deltaTime)
{
	// Logique de mise à jour du monde
}

void World::shutdown()
{
	// Nettoyage de nos objets alloués
	if (m_navQuery)
	{
		dtFreeNavMeshQuery(m_navQuery);
		m_navQuery = nullptr;
	}
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

Vector3 World::FindNearestPoint(const Vector3& point)
{
	if (!m_navQuery || !m_filter)
	{
		Logger::Log("FindNearestPoint Error: NavQuery or Filter not initialized.", LogType::Error);
		return point; // Retourne le point original
	}

	dtPolyRef ref;
	float nearestPt[3];
	float startPos[3] = { point.x, point.y, point.z };

	dtStatus status = m_navQuery->findNearestPoly(startPos, DEFAULT_SEARCH_EXTENTS, m_filter, &ref, nearestPt);

	if (dtStatusFailed(status) || ref == 0)
	{
		Logger::Log(std::format("FindNearestPoint Error: Could not find nearest poly for point ({},{},{}).", point.x, point.y, point.z), LogType::Error);
		return point; // Retourne le point original si échec
	}

	return { nearestPt[0], nearestPt[1], nearestPt[2] };
}


std::vector<Vector3> World::CalculatePath(const Vector3& start, const Vector3& end)
{
	std::vector<Vector3> pathPoints;
	if (!m_navQuery || !m_filter)
	{
		Logger::Log("CalculatePath Error: NavMesh components not initialized.", LogType::Error);
		return pathPoints;
	}

	dtPolyRef startRef, endRef;
	float startPos[3] = { start.x, start.y, start.z };
	float endPos[3] = { end.x, end.y, end.z };
	float nearestStart[3], nearestEnd[3];

	// Trouve le polygone le plus proche pour le point de départ
	dtStatus status = m_navQuery->findNearestPoly(startPos, DEFAULT_SEARCH_EXTENTS, m_filter, &startRef, nearestStart);
	if (dtStatusFailed(status) || startRef == 0)
	{
		Logger::Log(std::format("CalculatePath Error: Could not find nearest poly for start point. Status: 0x{:x}, Ref: {}", (unsigned int)status, startRef), LogType::Error);
		return pathPoints;
	}

	// Trouve le polygone le plus proche pour le point d'arrivée
	status = m_navQuery->findNearestPoly(endPos, DEFAULT_SEARCH_EXTENTS, m_filter, &endRef, nearestEnd);
	if (dtStatusFailed(status) || endRef == 0)
	{
		Logger::Log(std::format("CalculatePath Error: Could not find nearest poly for end point. Status: 0x{:x}, Ref: {}", (unsigned int)status, endRef), LogType::Error);
		return pathPoints;
	}

	const int MAX_POLYS = 256;
	dtPolyRef polys[MAX_POLYS];
	int npolys;

	// Calcule le chemin brut (une liste de polygones)
	// IMPORTANT: On utilise nearestStart et nearestEnd, pas startPos et endPos,
	// pour garantir que le chemin commence et se termine sur le navmesh.
	status = m_navQuery->findPath(startRef, endRef, nearestStart, nearestEnd, m_filter, polys, &npolys, MAX_POLYS);
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

	// Calcule le chemin "lissé" (une liste de points de passage)
	status = m_navQuery->findStraightPath(nearestStart, nearestEnd, polys, npolys,
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

Vector3 World::getRandomNavMeshPoint()
{
	if (!m_navQuery || !m_filter) {
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

	dtStatus status = m_navQuery->findRandomPoint(m_filter, frand, &randomRef, randomPt);

	if (dtStatusSucceed(status))
	{
		return { randomPt[0], randomPt[1], randomPt[2] };
	}

	return { 0, 0, 0 };
}