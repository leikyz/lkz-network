#pragma once

#include <vector>

// Forward declarations to avoid including heavy headers here.
struct Vector3;
struct dtNavMesh;

class World
{
public:
	World() = default;
	~World() = default;

	/// <summary>
	/// Initializes the world, loads the NavMesh from file.
	/// </summary>
	void initialize();

	/// <summary>
	/// Updates the world state (placeholder for future logic).
	/// </summary>
	/// <param name="deltaTime">Time elapsed since the last frame.</param>
	void update(double deltaTime);

	/// <summary>
	/// Cleans up resources, specifically the NavMesh.
	/// </summary>
	void shutdown();

	/// <summary>
	/// Calculates a path from a start to an end point using the loaded NavMesh.
	/// </summary>
	/// <param name="start">The starting position of the path.</param>
	/// <param name="end">The desired end position of the path.</param>
	/// <returns>A vector of points representing the path, or an empty vector if no path is found.</returns>
	std::vector<Vector3> CalculatePath(const Vector3& start, const Vector3& end);

private:
	dtNavMesh* navMesh = nullptr;
};

