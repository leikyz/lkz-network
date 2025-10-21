#include "LKZ/Simulation/Navmesh/NavMeshLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <cmath>

#include "Recast.h"
#include "RecastAlloc.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"

NavMeshLoader::~NavMeshLoader()
{
}

bool NavMeshLoader::LoadFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Can't open the file: " << path << std::endl;
        return false;
    }

    m_vertices.clear();
    m_triangles.clear();

    std::string line;
    std::stringstream ss;

    // --- Vertices ---
    int vertexCount = 0;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;
        vertexCount = std::stoi(line);
        break;
    }

    if (vertexCount <= 0) {
        std::cerr << "Error: Invalid or missing vertex count." << std::endl;
        return false;
    }

    m_vertices.reserve(vertexCount);
    for (int i = 0; i < vertexCount; ++i)
    {
        if (!std::getline(file, line)) {
            std::cerr << "Error: Unexpected end of file while reading vertices." << std::endl;
            return false;
        }
        ss.str(line);
        ss.clear();
        Vertex v;
        char comma;
        ss >> v.x >> comma >> v.y >> comma >> v.z;
        m_vertices.push_back(v);
    }

    // --- Triangle Reading ---
    int triangleCount = 0;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;
        triangleCount = std::stoi(line);
        break; // Found the count, break out
    }

    if (triangleCount <= 0) {
        std::cerr << "Error: Invalid or missing triangle count." << std::endl;
        return false;
    }

    m_triangles.reserve(triangleCount);
    for (int i = 0; i < triangleCount; ++i)
    {
        if (!std::getline(file, line)) {
            std::cerr << "Error: Unexpected end of file while reading triangles." << std::endl;
            return false;
        }
        ss.str(line);
        ss.clear();
        Triangle t;
        char comma;
        ss >> t.a >> comma >> t.b >> comma >> t.c;
        m_triangles.push_back(t);
    }

 /*   std::cout << "NavMesh loaded: " << m_vertices.size() << " vertices, " << m_triangles.size() << " triangles." << std::endl;*/
    return true;
}


dtNavMesh* NavMeshLoader::BuildNavMesh()
{
    if (m_vertices.empty() || m_triangles.empty())
    {
        std::cerr << "Error: No geometry loaded to build the NavMesh." << std::endl;
        return nullptr;
    }

    // Convert our structures to simple arrays that Recast can use
    std::vector<float> verts(m_vertices.size() * 3);
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        verts[i * 3 + 0] = m_vertices[i].x;
        verts[i * 3 + 1] = m_vertices[i].y;
        verts[i * 3 + 2] = m_vertices[i].z;
    }

    std::vector<int> tris(m_triangles.size() * 3);
    for (size_t i = 0; i < m_triangles.size(); ++i) {
        tris[i * 3 + 0] = m_triangles[i].a;
        tris[i * 3 + 1] = m_triangles[i].b;
        tris[i * 3 + 2] = m_triangles[i].c;
    }
    int nverts = static_cast<int>(m_vertices.size());
    int ntris = static_cast<int>(m_triangles.size());

    // --- Recast Configuration ---
    rcConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.cs = 0.3f; // Cell size (XZ)
    cfg.ch = 0.2f; // Cell height (Y)
    cfg.walkableSlopeAngle = 45.0f; // Maximum walkable slope angle in degrees
    cfg.walkableHeight = (int)ceilf(2.0f / cfg.ch); // Agent height
    cfg.walkableClimb = (int)floorf(0.9f / cfg.ch); // Maximum height the agent can climb
    cfg.walkableRadius = (int)ceilf(0.2f / cfg.cs); // Agent radius
    cfg.maxEdgeLen = (int)(12.0f / cfg.cs); // Maximum edge length
    cfg.maxSimplificationError = 1.3f;
    cfg.minRegionArea = (int)rcSqr(8); // Minimum region area
    cfg.mergeRegionArea = (int)rcSqr(20); // Region area below which it will be merged
    cfg.maxVertsPerPoly = 6;
    cfg.detailSampleDist = 6.0f;
    cfg.detailSampleMaxError = 1.0f;
    cfg.tileSize = 0; // 0 for a non-tiled NavMesh

    // Calculate the Bounding Box of the geometry
    rcCalcBounds(verts.data(), nverts, cfg.bmin, cfg.bmax);

    // Calculate grid size
    rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

    rcContext ctx;

    // --- Recast Processing Pipeline ---

    // 1. Allocate and rasterize the Heightfield
    rcHeightfield* solid = rcAllocHeightfield();
    if (!rcCreateHeightfield(&ctx, *solid, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch)) {
        std::cerr << "Error: rcCreateHeightfield failed." << std::endl;
        rcFreeHeightField(solid);
        return nullptr;
    }

    std::vector<unsigned char> triareas(ntris, RC_WALKABLE_AREA);
    if (!rcRasterizeTriangles(&ctx, verts.data(), nverts, tris.data(), triareas.data(), ntris, *solid, cfg.walkableClimb)) {
        std::cerr << "Error: rcRasterizeTriangles failed." << std::endl;
        rcFreeHeightField(solid);
        return nullptr;
    }

    // 2. Filter walkable surfaces
    rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *solid);
    rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *solid);
    rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *solid);

    // 3. Partition the surface into regions
    rcCompactHeightfield* chf = rcAllocCompactHeightfield();
    if (!rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb, *solid, *chf)) {
        std::cerr << "Error: rcBuildCompactHeightfield failed." << std::endl;
        rcFreeHeightField(solid);
        rcFreeCompactHeightfield(chf);
        return nullptr;
    }
    rcFreeHeightField(solid); // We no longer need the initial heightfield
    solid = nullptr;

    if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *chf)) {
        std::cerr << "Error: rcErodeWalkableArea failed." << std::endl;
        rcFreeCompactHeightfield(chf);
        return nullptr;
    }

    // Use the Watershed partitioning, it's the most robust
    if (!rcBuildDistanceField(&ctx, *chf)) {
        std::cerr << "Error: rcBuildDistanceField failed." << std::endl;
        rcFreeCompactHeightfield(chf);
        return nullptr;
    }
    if (!rcBuildRegions(&ctx, *chf, 0, cfg.minRegionArea, cfg.mergeRegionArea)) {
        std::cerr << "Error: rcBuildRegions failed." << std::endl;
        rcFreeCompactHeightfield(chf);
        return nullptr;
    }

    // 4. Create region contours
    rcContourSet* cset = rcAllocContourSet();
    if (!rcBuildContours(&ctx, *chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *cset)) {
        std::cerr << "Error: rcBuildContours failed." << std::endl;
        rcFreeCompactHeightfield(chf);
        rcFreeContourSet(cset);
        return nullptr;
    }

    // 5. Build the polygon mesh
    rcPolyMesh* pmesh = rcAllocPolyMesh();
    if (!rcBuildPolyMesh(&ctx, *cset, cfg.maxVertsPerPoly, *pmesh)) {
        std::cerr << "Error: rcBuildPolyMesh failed." << std::endl;
        rcFreeCompactHeightfield(chf);
        rcFreeContourSet(cset);
        rcFreePolyMesh(pmesh);
        return nullptr;
    }

    // 6. Build the detail mesh
    rcPolyMeshDetail* dmesh = rcAllocPolyMeshDetail();
    if (!rcBuildPolyMeshDetail(&ctx, *pmesh, *chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *dmesh)) {
        std::cerr << "Error: rcBuildPolyMeshDetail failed." << std::endl;
        rcFreeCompactHeightfield(chf);
        rcFreeContourSet(cset);
        rcFreePolyMesh(pmesh);
        rcFreePolyMeshDetail(dmesh);
        return nullptr;
    }

    // We can free the intermediate structures
    rcFreeCompactHeightfield(chf);
    rcFreeContourSet(cset);
    chf = nullptr;
    cset = nullptr;

    // --- Detour NavMesh Construction ---

    if (cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON) {
        unsigned char* navData = 0;
        int navDataSize = 0;

        dtNavMeshCreateParams params;
        memset(&params, 0, sizeof(params));
        params.verts = pmesh->verts;
        params.vertCount = pmesh->nverts;
        params.polys = pmesh->polys;
        params.polyAreas = pmesh->areas;
        params.polyFlags = pmesh->flags;
        params.polyCount = pmesh->npolys;
        params.nvp = pmesh->nvp;
        params.detailMeshes = dmesh->meshes;
        params.detailVerts = dmesh->verts;
        params.detailVertsCount = dmesh->nverts;
        params.detailTris = dmesh->tris;
        params.detailTriCount = dmesh->ntris;
        params.offMeshConVerts = 0;
        params.offMeshConRad = 0;
        params.offMeshConFlags = 0;
        params.offMeshConAreas = 0;
        params.offMeshConDir = 0;
        params.offMeshConUserID = 0;
        params.offMeshConCount = 0;
        params.walkableHeight = (float)cfg.walkableHeight * cfg.ch;
        params.walkableRadius = (float)cfg.walkableRadius * cfg.cs;
        params.walkableClimb = (float)cfg.walkableClimb * cfg.ch;
        rcVcopy(params.bmin, pmesh->bmin);
        rcVcopy(params.bmax, pmesh->bmax);
        params.cs = cfg.cs;
        params.ch = cfg.ch;
        params.buildBvTree = true;

        if (!dtCreateNavMeshData(&params, &navData, &navDataSize)) {
            std::cerr << "Error: dtCreateNavMeshData failed." << std::endl;
            rcFreePolyMesh(pmesh);
            rcFreePolyMeshDetail(dmesh);
            return nullptr;
        }

        dtNavMesh* navMesh = dtAllocNavMesh();
        if (!navMesh) {
            std::cerr << "Error: dtAllocNavMesh failed." << std::endl;
            dtFree(navData);
            rcFreePolyMesh(pmesh);
            rcFreePolyMeshDetail(dmesh);
            return nullptr;
        }

        dtStatus status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
        if (dtStatusFailed(status)) {
            std::cerr << "Error: navMesh->init failed." << std::endl;
            dtFree(navData);
            dtFreeNavMesh(navMesh);
            rcFreePolyMesh(pmesh);
            rcFreePolyMeshDetail(dmesh);
            return nullptr;
        }

      /*  std::cout << "NavMesh built successfully!" << std::endl;*/

        // The remaining Recast data is now useless
        rcFreePolyMesh(pmesh);
        rcFreePolyMeshDetail(dmesh);

        return navMesh;
    }

    // Cleanup on error
    rcFreePolyMesh(pmesh);
    rcFreePolyMeshDetail(dmesh);
    return nullptr;
}
