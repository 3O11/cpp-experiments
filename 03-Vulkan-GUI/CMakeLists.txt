include(${CMAKE_SOURCE_DIR}/cmake/glfw.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/imgui.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/stb.cmake)
find_package(Vulkan REQUIRED)

add_executable(03-Vulkan-GUI
    "main.cpp"
)

set_property(TARGET 03-Vulkan-GUI PROPERTY CXX_STANDARD 20)

#target_compile_options(03-Vulkan-GUI PRIVATE "-fsanitize=address")

target_link_libraries(03-Vulkan-GUI
    glfw
    stb_image
    imgui-glfw-vulkan
#    "-fsanitize=address"
)
