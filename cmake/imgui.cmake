include(FetchContent)
include(${CMAKE_SOURCE_DIR}/cmake/metal-cpp.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/glfw.cmake)

set(IMGUI_BASE_DIR ${FETCHCONTENT_BASE_DIR}/imgui-src)
# Not needed, since it is equivalent to the very similar variable set by
# FetchContent_Declare (imgui_SOURCE_DIR).
#set(IMGUI_SOURCE_DIR ${IMGUI_BASE_DIR}/imgui)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/3O11/imgui
    SOURCE_DIR     IMGUI_SOURCE_DIR
)

if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    add_library(imgui-glfw-opengl STATIC)
    add_library(imgui-glfw-metal STATIC)

    ##########################################################################
    # Common settings
    ##########################################################################

    set(IMGUI_COMMON_SOURCES
        "${imgui_SOURCE_DIR}/imgui.h"
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_internal.h"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    )

    set(IMGUI_COMMON_GLFW
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    )

    ##########################################################################
    # GLFW + OpenGL config
    ##########################################################################

    target_sources(imgui-glfw-opengl
        PRIVATE
        ${IMGUI_COMMON_SOURCES}
        ${IMGUI_COMMON_GLFW}
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
    )

    target_include_directories(imgui-glfw-opengl
        PUBLIC
        ${IMGUI_BASE_DIR}
        PRIVATE
        ${imgui_SOURCE_DIR}
    )

    set_target_properties(imgui-glfw-opengl
        PROPERTIES
        CXX_STANDARD 20
    )

    target_link_libraries(imgui-glfw-opengl
        PRIVATE
        glfw
    )

    ##########################################################################
    # GLFW + Metal config
    ##########################################################################

    target_sources(imgui-glfw-metal
        PRIVATE
        ${IMGUI_COMMON_SOURCES}
        ${IMGUI_COMMON_GLFW}
        "${imgui_SOURCE_DIR}/backends/imgui_impl_metal.h"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm"
    )

    target_compile_definitions(imgui-glfw-metal
        PRIVATE
        "IMGUI_IMPL_METAL_CPP"
    )

    set_target_properties(imgui-glfw-metal
        PROPERTIES
        CXX_STANDARD 20
    )

    target_include_directories(imgui-glfw-metal
        PUBLIC
        ${IMGUI_BASE_DIR}
        PRIVATE
        ${imgui_SOURCE_DIR}
    )

    target_link_libraries(imgui-glfw-metal
        PRIVATE
        glfw
        metal-cpp
        "-framework Metal"
        "-framework MetalKit"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework QuartzCore"
    )
endif()
