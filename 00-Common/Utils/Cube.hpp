#ifndef EXP_UTILS_CUBE_HPP
#define EXP_UTILS_CUBE_HPP

#include "ObjectData.hpp"

#include <vector>

namespace Utils
{
    ObjectData MakeCube()
    {
        ObjectData result;

        // Front face
        result.Vertices.push_back({{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}});
        result.Vertices.push_back({{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}});
        result.Vertices.push_back({{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}});
        result.Vertices.push_back({{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}});

        // Back face
        result.Vertices.push_back({{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}});
        result.Vertices.push_back({{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}});
        result.Vertices.push_back({{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}});
        result.Vertices.push_back({{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}});

        // Top face
        result.Vertices.push_back({{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}});
        result.Vertices.push_back({{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}});
        result.Vertices.push_back({{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}});
        result.Vertices.push_back({{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}});

        // Bottom face
        result.Vertices.push_back({{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}});
        result.Vertices.push_back({{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}});
        result.Vertices.push_back({{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}});
        result.Vertices.push_back({{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}});

        // Right face
        result.Vertices.push_back({{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}});
        result.Vertices.push_back({{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}});
        result.Vertices.push_back({{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}});
        result.Vertices.push_back({{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}});

        // Left face
        result.Vertices.push_back({{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}});
        result.Vertices.push_back({{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}});
        result.Vertices.push_back({{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}});
        result.Vertices.push_back({{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}});

        result.Indices = {
             0,  1,  2,  0,  2,  3, // Front face
             4,  5,  6,  4,  6,  7, // Back face
             8,  9, 10,  8, 10, 11, // Top face
            12, 13, 14, 12, 14, 15, // Bottom face
            16, 17, 18, 16, 18, 19, // Rigth face
            20, 21, 22, 20, 22, 23, // Left face
        };

        return result;
    }
}

#endif //EXP_UTILS_CUBE_HPP
