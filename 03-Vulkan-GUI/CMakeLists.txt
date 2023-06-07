include(${CMAKE_SOURCE_DIR}/cmake/glfw.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/vulkan.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/imgui.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/stb.cmake)

add_executable(03-Vulkan-GUI
    "main.cpp"
)

set_property(TARGET 03-Vulkan-GUI PROPERTY CXX_STANDARD 20)

if(APPLE)
    target_link_libraries(03-Vulkan-GUI
        PRIVATE
        MoltenVK
    )
else()
    target_link_libraries(03-Vulkan-GUI
        PRIVATE
        Vulkan::Headers
        Vulkan::Vulkan
    )
endif()

target_link_libraries(03-Vulkan-GUI
    PRIVATE
    glfw
    stb_image

    VulkanMemoryAllocator
    imgui-glfw-vulkan

    shaderc
    SPIRV-Headers
    SPIRV-Tools
    SPIRV-Tools-opt
    SPIRV-Cross
)
