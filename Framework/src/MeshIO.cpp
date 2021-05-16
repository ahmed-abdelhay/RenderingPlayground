#include "Resha.h"

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include <robin_hood.h>

#ifdef RENDER_OS_WINDOWS
#define PACKED_STRUCT 
#else
#define PACKED_STRUCT __attribute__((packed))
#endif
namespace
{
    struct Buffer
    {
        const uint8_t* data = nullptr;
        size_t size = 0;
        size_t cursor = 0;
    };

#pragma pack(push, 1)
    struct PACKED_STRUCT FaceInfo
    {
        Vec3f normal;
        Vec3f points[3];
        uint16_t attributes;
    };

    template <typename T>
    bool ReadFromBuffer(Buffer& buffer, T& result)
    {
        const size_t byetsToRead = sizeof(T);
        if ((buffer.cursor + byetsToRead) >= buffer.size)
        {
            return false;
        }
        memcpy(&result, buffer.data + buffer.cursor, byetsToRead);
        buffer.cursor += byetsToRead;
        return true;
    }

    template <typename T>
    void AppendData(const T& buffer, std::vector<uint8_t>& data)
    {
        for (int i = 0; i < sizeof(T); i++)
        {
            data.push_back(((uint8_t*)&buffer)[i]);
        }
    }

    struct Vec3fHash
    {
        size_t operator()(const Vec3f& v) const
        {
            robin_hood::hash<float> hash;
            return hash(v.x) ^ hash(v.y) ^ hash(v.z);
        }
    };
} // namespace

bool WriteStl(const SurfaceMesh& mesh, const char* fileName)
{
    std::vector<uint8_t>data;
    data.resize(80);

    const uint32_t facesCount = mesh.faces.size();
    AppendData(facesCount, data);
    for (size_t i = 0; i < facesCount; ++i)
    {
        const Vec3d& v0 = mesh.vertices[mesh.faces[i].idx[0]];
        const Vec3d& v1 = mesh.vertices[mesh.faces[i].idx[1]];
        const Vec3d& v2 = mesh.vertices[mesh.faces[i].idx[2]];
        assert(!(isnan(v0.x) || isnan(v0.y) || isnan(v0.z)));
        assert(!(isnan(v1.x) || isnan(v1.y) || isnan(v1.z)));
        assert(!(isnan(v2.x) || isnan(v2.y) || isnan(v2.z)));

        const Vec3d dir0 = v1 - v0;
        const Vec3d dir1 = v2 - v0;
        Vec3d n = CrossProduct(dir0, dir1);
        Normalise(n);
        assert(!(isnan(n.x) || isnan(n.y) || isnan(n.z)));

        AppendData(n, data);
        AppendData(v0, data);
        AppendData(v1, data);
        AppendData(v2, data);
        AppendData(uint16_t(0), data);
    }
    return WriteFile(fileName, data.data(), data.size());
}

IOStatus ReadMesh(const char* fileName, SurfaceMesh& result)
{
    std::vector<uint8_t> data;
    if (!ReadFile(fileName, data))
    {
        return IOStatus::FILE_DOESNT_EXIST;
    }
    robin_hood::unordered_map<Vec3f, uint32_t, Vec3fHash> pointsMap;
    auto InsertVertex = [&](const Vec3f& p)
    {
        const auto itr = pointsMap.find(p);
        if (itr != pointsMap.end())
        {
            return itr->second;
        }
        const uint32_t idx = result.vertices.size();
        result.vertices.push_back(Vec3d{ p.x, p.y, p.z });
        pointsMap[p] = idx;
        return idx;
    };
    Buffer buffer;
    buffer.data = data.data();
    buffer.size = data.size();
    buffer.cursor = 80; // header size

    uint32_t facesCount;
    ReadFromBuffer(buffer, facesCount);

    result.faces.reserve(facesCount);
    FaceInfo info;
    for (size_t i = 0; i < facesCount; ++i)
    {
        ReadFromBuffer(buffer, info);
        Triangle t;
        t.idx[0] = InsertVertex(info.points[0]);
        t.idx[1] = InsertVertex(info.points[1]);
        t.idx[2] = InsertVertex(info.points[2]);
        result.faces.push_back(t);
    }
    result.name = ExtractFileName(fileName);
    result.color = GenerateColor();
    result.id = GenerateUUID();
    return IOStatus::OK;
}
