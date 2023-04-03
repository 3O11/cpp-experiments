include(${CMAKE_SOURCE_DIR}/cmake/glfw.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/metal-cpp.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/imgui.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/stb.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/mathlib.cmake)

add_executable(10-Metal-Rendering
    "metalUtil.h"
    "metalUtil.mm"
    "main.cpp"
)

set_property(TARGET 10-Metal-Rendering PROPERTY CXX_STANDARD 20)

target_compile_options(10-Metal-Rendering PRIVATE "-fsanitize=address")

target_link_libraries(10-Metal-Rendering
    glfw
    metal-cpp
    stb_image
    imgui-glfw-metal
    MathLib
    CommonUtils
    "-framework Metal"
    "-framework MetalKit"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework QuartzCore"
    "-fsanitize=address"
)

configure_file(${CMAKE_SOURCE_DIR}/10-Metal-Rendering/shader.metal ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
