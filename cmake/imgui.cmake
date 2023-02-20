include(FetchContent)

FecthContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/3O11/imgui
)

FetchContent_Populate(
    imgui
)

add_library(imgui-glfw-opengl STATIC)
add_library(imgui-glfw-metal STATIC)

##############################################################################
# Common settings
##############################################################################

set(IMGUI_COMMON_SOURCES
    "imgui/imgui.h"
    "imgui/imgui.cpp"
    "imgui/imgui_internal.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_tables.cpp"
    "imgui/imgui_widgets.cpp"
)

set(IMGUI_COMMON_GLFW
    "imgui/backends/imgui_impl_glfw.h"
    "imgui/backends/imgui_impl_glfw.cpp"
)

##############################################################################
# GLFW + OpenGL config
##############################################################################

target_sources(imgui-glfw-opengl
    PRIVATE
    ${IMGUI_COMMON_SOURCES}
    ${IMGUI_COMMON_GLFW}
    "imgui/backends/imgui_impl_opengl3.h"
    "imgui/backends/imgui_impl_opengl3.cpp"
)

##############################################################################
# GLFW + Metal config
##############################################################################

target_sources(imgui-glfw-metal
    PRIVATE
    ${IMGUI_COMMON_SOURCES}
    ${IMGUI_COMMON_GLFW}
    "imgui/backends/imgui_impl_metal.h"
    "imgui/backends/imgui_impl_metal.mm"
)

set_target_properties(imgui-glfw-metal
    PROPERTIES
    COMPILE_DEFINITIONS
    "IMGUI_IMPL_METAL_CPP"
)
