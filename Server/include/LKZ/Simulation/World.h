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

	Vector3 FindNearestPoint(dtNavMeshQuery* navQuery, const Vector3& point);

	std::vector<Vector3> CalculatePath(dtNavMeshQuery* navQuery, const Vector3& start, const Vector3& end);

	Vector3 getRandomNavMeshPoint(dtNavMeshQuery* navQuery);

	dtNavMesh* getNavMesh() const { return navMesh; }

private:
	dtNavMesh* navMesh = nullptr;
	dtQueryFilter* m_filter = nullptr;
};
