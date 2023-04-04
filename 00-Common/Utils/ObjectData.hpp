#ifndef EXP_UTILS_VERTEX_HPP
#define EXP_UTILS_VERTEX_HPP

#include <Math/Vector.hpp>

#include <vector>

namespace Utils
{
    struct Vertex
    {
        Math::Vector3f Position;
        Math::Vector3f Normal;
        Math::Vector2f TexCoords;
    };

    struct AlignedVertex
    {
        Math::Vector4f Position;
        Math::Vector4f Normal;
        Math::Vector4f TexCoords;
    };

    struct ObjectData
    {
        std::vector<Vertex>   Vertices;
        std::vector<uint16_t> Indices;
    };

    struct AlignedObjectData
    {
        std::vector<AlignedVertex> Vertices;
        std::vector<uint16_t> Indices;
    };

    AlignedObjectData Align(const ObjectData& data)
    {
        AlignedObjectData result;

        result.Indices = data.Indices;
        result.Vertices.reserve(data.Vertices.size());

        for(auto&& vertex : data.Vertices)
        {
            result.Vertices.push_back(AlignedVertex{
                .Position = Math::Vector4f(vertex.Position),
                .Normal = Math::Vector4f(vertex.Normal),
                .TexCoords = Math::Vector4f(vertex.TexCoords)
            });
        }

        return result;
    }
}

#endif //EXP_UTILS_VERTEX_HPP
