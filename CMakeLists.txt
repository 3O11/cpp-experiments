cmake_minimum_required(VERSION 3.20)

project(cpp-experiments
    LANGUAGES C CXX
)

add_subdirectory(00-Common)
add_subdirectory(01-Metal-GUI)
add_subdirectory(02-OpenGL-GUI)
add_subdirectory(03-Vulkan-GUI)

add_subdirectory(10-Metal-Rendering)
