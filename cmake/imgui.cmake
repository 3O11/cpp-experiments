include(FetchContent)
include(${CMAKE_SOURCE_DIR}/cmake/glad.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/vulkan.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/metal-cpp.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/glfw.cmake)

set(IMGUI_BASE_DIR ${FETCHCONTENT_BASE_DIR}/imgui-src)
set(IMGUI_SOURCE_DIR ${IMGUI_BASE_DIR}/imgui)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/3O11/imgui
    GIT_TAG        192196711a7d0d7c2d60454d42654cf090498a74 # Version 1.89.3 on the docking branch
    SOURCE_DIR     ${IMGUI_SOURCE_DIR}
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)

    FetchContent_Populate(imgui)

    add_library(imgui-glfw-opengl STATIC)
    add_library(imgui-glfw-metal STATIC)
    add_library(imgui-glfw-vulkan STATIC)

    ##########################################################################
    # Common settings
    ##########################################################################

    set(IMGUI_COMMON_SOURCES
        ${IMGUI_SOURCE_DIR}/imgui.h
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_internal.h
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
        ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
    )

    set(IMGUI_COMMON_GLFW
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    )

    ##########################################################################
    # GLFW + OpenGL config
    ##########################################################################

    target_sources(imgui-glfw-opengl
        PRIVATE
        ${IMGUI_COMMON_SOURCES}
        ${IMGUI_COMMON_GLFW}
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )

    set_target_properties(imgui-glfw-opengl PROPERTIES
        CXX_STANDARD 20
    )

    target_include_directories(imgui-glfw-opengl
        PUBLIC
        ${IMGUI_BASE_DIR}
        ${imgui_SOURCE_DIR}
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
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_metal.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_metal.mm
    )

    set_target_properties(imgui-glfw-metal PROPERTIES
        CXX_STANDARD 20
    )

    target_include_directories(imgui-glfw-metal
        PUBLIC
        ${IMGUI_BASE_DIR}
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

    target_compile_definitions(imgui-glfw-metal
        PRIVATE
        "IMGUI_IMPL_METAL_CPP"
    )

    ##########################################################################
    # GLFW + Vulkan config
    ##########################################################################

    target_sources(imgui-glfw-vulkan
        PRIVATE
        ${IMGUI_COMMON_SOURCES}
        ${IMGUI_COMMON_GLFW}
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.h
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
    )

    set_target_properties(imgui-glfw-vulkan PROPERTIES
        CXX_STANDARD 20
    )

    target_include_directories(imgui-glfw-vulkan
        PUBLIC
        ${IMGUI_BASE_DIR}
        ${imgui_SOURCE_DIR}
    )

    target_link_libraries(imgui-glfw-vulkan
        PRIVATE
        glfw
    )

    if(APPLE)
        target_link_libraries(imgui-glfw-vulkan
            PRIVATE
            MoltenVK
        )
    else()
        target_link_libraries(imgui-glfw-vulkan
            PRIVATE
            Vulkan::Headers
            Vulkan::Vulkan
        )
    endif()

endif()
