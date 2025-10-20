#ifndef NAVMESHLOADER_H
#define NAVMESHLOADER_H

#include <string>
#include <vector>

// Forward declaration for Detour's navmesh struct
struct dtNavMesh;

// Simple struct to hold vertex data
struct Vertex {
    float x, y, z;
};

// Simple struct to hold triangle indices
struct Triangle {
    int a, b, c;
};

/**
 * @class NavMeshLoader
 * @brief Handles loading geometry from a file and building a Detour NavMesh.
 */
class NavMeshLoader
{
public:
    NavMeshLoader() = default;
    ~NavMeshLoader();

    /**
     * @brief Loads vertex and triangle data from a custom text file format.
     * @param path The path to the geometry file.
     * @return True if loading is successful, false otherwise.
     */
    bool LoadFromFile(const std::string& path);

    /**
     * @brief Builds a dtNavMesh object from the loaded geometry using the Recast pipeline.
     * @return A pointer to the generated dtNavMesh, or nullptr on failure.
     */
    dtNavMesh* BuildNavMesh();

private:
    std::vector<Vertex> m_vertices;
    std::vector<Triangle> m_triangles;
};

#endif // NAVMESHLOADER_H
