#pragma once

#include <vector>

struct Vector3;
struct dtNavMesh;
class dtNavMeshQuery; 
class dtQueryFilter; 

class World
{
public:
	World() = default;
	~World() = default;

	void initialize();
	void update(double deltaTime);
	void shutdown();

	Vector3 FindNearestPoint(const Vector3& point);

	std::vector<Vector3> CalculatePath(const Vector3& start, const Vector3& end);

	Vector3 getRandomNavMeshPoint();

private:
	dtNavMesh* navMesh = nullptr;
	dtNavMeshQuery* m_navQuery = nullptr;
	dtQueryFilter* m_filter = nullptr;
};