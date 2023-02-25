include(${CMAKE_SOURCE_DIR}/cmake/glfw.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/metal-cpp.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/imgui.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/stb.cmake)

add_executable(01-Metal-GUI
    "metalUtil.h"
    "metalUtil.mm"
    "main.cpp"
)

set_property(TARGET 01-Metal-GUI PROPERTY CXX_STANDARD 20)

target_compile_options(01-Metal-GUI PRIVATE "-fsanitize=address")

target_link_libraries(01-Metal-GUI
    glfw
    metal-cpp
    stb_image
    imgui-glfw-metal
    "-framework Metal"
    "-framework MetalKit"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework QuartzCore"
    "-fsanitize=address"
)
