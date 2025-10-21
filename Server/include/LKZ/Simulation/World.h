#pragma once

#include <vector>


class NavMeshLoader;
struct Vector3;
struct dtNavMesh;

class World
{
public:
	World() = default;
	~World() = default;
	void initialize();
	void update(double deltaTime);
	void shutdown();
	std::vector<Vector3> CalculatePath(const Vector3& start, const Vector3& end);
private:
	dtNavMesh* navMesh;
};