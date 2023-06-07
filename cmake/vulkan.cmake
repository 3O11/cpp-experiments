include(FetchContent)

cmake_policy(SET CMP0135 NEW)

##############################################################################
# Declare Vulkan repositories
##############################################################################

# TODO: Figure out how to add profiles and other tools

set(VULKAN_SDK_VERSION sdk-1.3.243.0)
set(SHADERC_VERSION    v2023.3)
set(MOLTENVK_VERSION   v1.2.4)

# Basic Vulkan stuff

FetchContent_Declare(
    vulkan-headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    vulkan-loader
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

# Shader compilers

FetchContent_Declare(
    glslang
    GIT_REPOSITORY https://github.com/KhronosGroup/glslang
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    shaderc
    GIT_REPOSITORY https://github.com/google/shaderc
    GIT_TAG        ${SHADERC_VERSION}
)

# SPIRV

FetchContent_Declare(
    spirv-headers
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    spirv-tools
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    spirv-cross
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    spirv-reflect
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

# Other stuff

FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
)

FetchContent_Declare(
    moltenvk
    URL https://github.com/KhronosGroup/moltenvk/releases/download/v1.2.4/moltenvk-macos.tar
)

##############################################################################
# Set options to minimize the amount of available targets
##############################################################################

# shaderc
set(SHADERC_SKIP_TESTS    ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)

# SPIRV-Headers
set(SPIRV_HEADERS_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SPIRV_HEADERS_SKIP_INSTALL  ON CACHE BOOL "" FORCE)

# SPIRV-Cross
set(SPIRV_CROSS_CLI          OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)

# SPIRV-Reflect
set(SPIRV_REFLECT_STATIC_LIB ON  CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_EXAMPLES   OFF CACHE BOOL "" FORCE)

# volk
set(VOLK_STATIC_DEFINES ON  CACHE BOOL "" FORCE)
set(VOLK_PULL_IN_VULKAN OFF CACHE BOOL "" FORCE)

# Make necessary targets available
FetchContent_MakeAvailable(
    # Base libraries
    vulkan-headers
    vulkan-loader

    # SPIRV manipulation libraries
    spirv-headers
    spirv-tools
    spirv-cross
    spirv-reflect

    # Shader compilers
    glslang
    shaderc

    # Other
    volk
)

FetchContent_GetProperties(vma)
if(NOT vma_POPULATED)
    FetchContent_Populate(vma)

    add_library(VulkanMemoryAllocator INTERFACE)
    target_include_directories(VulkanMemoryAllocator
        INTERFACE
        ${VMA_SOURCE_DIR}/include
    )
endif()

FetchContent_GetProperties(moltenvk)
if(NOT moltenvk_POPULATED)
    FetchContent_Populate(moltenvk)
endif()

if(NOT TARGET MoltenVK)
    add_library(MoltenVK STATIC IMPORTED)
    target_include_directories(MoltenVK
        INTERFACE
        ${moltenvk_SOURCE_DIR}/MoltenVK/include
    )
    set_target_properties(MoltenVK PROPERTIES
        IMPORTED_LOCATION ${moltenvk_SOURCE_DIR}/MoltenVK/MoltenVK.xcframework/macos-arm64_x86_64/libMoltenVK.a
    )
endif()

if(NOT TARGET SPIRV-Cross)
    add_library(SPIRV-Cross INTERFACE)
    target_link_libraries(SPIRV-Cross
        INTERFACE
        spirv-cross-glsl
        spirv-cross-hlsl
        spirv-cross-msl
        spirv-cross-cpp
        spirv-cross-reflect
        spirv-cross-util
    )
endif()

if(NOT TARGET SPIRV-Reflect)
    add_library(SPIRV-Reflect ALIAS spirv-reflect-static)
endif()

target_link_libraries(volk
    PRIVATE
    Vulkan::Headers
    Vulkan::Vulkan
)
