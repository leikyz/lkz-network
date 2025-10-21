#pragma once

#include <string>
#include <vector>

struct dtNavMesh;
struct Vertex { float x, y, z; };

class NavMeshLoader
{
public:
    NavMeshLoader() = default;
    ~NavMeshLoader() = default;

    // Loads the geometry from a file exported by the Unity script.
    bool LoadFromFile(const std::string& path);

    // Builds the dtNavMesh object from the loaded geometry.
    dtNavMesh* BuildNavMesh();

private:
    std::vector<Vertex> m_vertices;
    std::vector<int> m_indices;
    std::vector<unsigned char> m_triAreas;
};