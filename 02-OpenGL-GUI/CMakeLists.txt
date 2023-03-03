include(${CMAKE_SOURCE_DIR}/cmake/glfw.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/glad.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/imgui.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/stb.cmake)

add_executable(02-OpenGL-GUI
    "main.cpp"
)

set_property(TARGET 02-OpenGL-GUI PROPERTY CXX_STANDARD 20)

#target_compile_options(02-OpenGL-GUI PRIVATE "-fsanitize=address")

target_link_libraries(02-OpenGL-GUI
    glad
    glfw
    stb_image
    imgui-glfw-opengl
#    "-fsanitize=address"
)
