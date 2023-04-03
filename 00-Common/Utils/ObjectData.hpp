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

    struct ObjectData
    {
        std::vector<Vertex>   Vertices;
        std::vector<uint16_t> Indices;
    };
}

#endif //EXP_UTILS_VERTEX_HPP
