#include "Resha.h"
#include <float.h>

std::vector<Vec3d> CalculateFacesNormals(const SurfaceMesh& mesh)
{
    std::vector<Vec3d> faceNormals;
    faceNormals.reserve(mesh.faces.size());
    for (const Triangle& t : mesh.faces)
    {
        const Vec3d v0 = mesh.vertices[t.idx[0]];
        const Vec3d v1 = mesh.vertices[t.idx[1]];
        const Vec3d v2 = mesh.vertices[t.idx[2]];
        faceNormals.push_back(Normalised(CrossProduct(v1 - v0, v2 - v0)));
    }
    return faceNormals;
}

std::vector<Vec3d> CalculateVertexNormals(const SurfaceMesh& mesh,
                                          const Connectivity& connectivity)
{
    const std::vector<Vec3d> faceNormals = CalculateFacesNormals(mesh);
    const size_t verticesCount = mesh.vertices.size();
    std::vector<Vec3d> normals(verticesCount);
    for (size_t i = 0; i < verticesCount; ++i)
    {
        normals[i] = Vec3d{ 0.0, 0.0, 0.0 };
        for (const size_t faceIdx : connectivity.pointCells[i].adjacentFaces)
        {
            normals[i] = normals[i] + faceNormals[faceIdx];
        }
        const size_t adjecantFacesCount = connectivity.pointCells[i].adjacentFaces.size();
        normals[i] = normals[i] * (1.0 / adjecantFacesCount);
        Normalise(normals[i]);
    }
    return normals;
}

BBox CalculateBoundingBox(const SurfaceMesh& mesh)
{
    BBox result;
    for (const Vec3d& v : mesh.vertices)
    {
        result.min.x = std::min(v.x, result.min.x);
        result.min.y = std::min(v.y, result.min.y);
        result.min.z = std::min(v.z, result.min.z);
        result.max.x = std::max(v.x, result.max.x);
        result.max.y = std::max(v.y, result.max.y);
        result.max.z = std::max(v.z, result.max.z);
    }
    return result;
}

Connectivity BuildConnectivity(const SurfaceMesh& mesh)
{
    Connectivity c;
    const size_t pointsCount = mesh.vertices.size();
    c.pointCells.resize(pointsCount);
    for (size_t i = 0; i < pointsCount; ++i)
    {
        c.pointCells[i].vertexIndex = i;
    }
    const size_t facesCount = mesh.faces.size();
    for (size_t i = 0; i < facesCount; ++i)
    {
        const Triangle& f = mesh.faces[i];
        c.pointCells[f.idx[0]].adjacentFaces.insert(i);
        c.pointCells[f.idx[1]].adjacentFaces.insert(i);
        c.pointCells[f.idx[2]].adjacentFaces.insert(i);
    }
    return c;
}
