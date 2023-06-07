include(FetchContent)

set(VULKAN_SDK_VERSION sdk-1.3.243.0)
set(SHADERC_VERSION    v2023.3)

# Vulkan SDK repos
#  - some of these are actually not necessary for development, they will be
#    in the future

FetchContent_Declare(
    VulkanHeaders
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    VulkanLoader
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Loader
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

#FetchContent_Declare(
#    VulkanValidationLayers
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-ValidationLayers
#    GIT_TAG        ${VULKAN_SDK_VERSION}
#)

#FetchContent_Declare(
#    VulkanExtensionLayer
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-ExtensionLayer
#    GIT_TAG        ${VULKAN_SDK_VERSION}
#)

#FetchContent_Declare(
#    VulkanProfiles
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Profiles
#    GIT_TAG        ${VULKAN_SDK_VERSION}
#)

#FetchContent_Declare(
#    VulkanTools
#    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Tools
#    GIT_TAG        ${VULKAN_SDK_VERSION}
#)

#FetchContent_Declare(
#    LunargVulkanTools
#    GIT_REPOSITORY https://github.com/LunarG/VulkanTools
#    GIT_TAG        ${VULKAN_SDK_VERSION}
#)

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
    SPIRVHeaders
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Headers
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    SPIRVTools
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Tools
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    SPIRVCross
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    SPIRVReflect
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    gfxreconstruct
    GIT_REPOSITORY https://github.com/LunarG/gfxreconstruct
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

# Other stuff

FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk
    GIT_TAG        ${VULKAN_SDK_VERSION}
)

FetchContent_Declare(
    VMA
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
)

#FetchContent_Declare(
#    VulkanCapsViewer
#    GIT_REPOSITORY https://github.com/SaschaWillems/VulkanCapsViewer
#    GIT_TAG        3.31
#)

##############################################################################
# Set options to minimize the amount of available targets
##############################################################################

# shaderc
set(SHADERC_SKIP_TESTS    ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)

# SPIRV-Headers
set(SPIRV_HEADERS_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)

# SPIRV-Cross
set(SPIRV_CROSS_CLI          OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)

# SPIRV-Reflect
set(SPIRV_REFLECT_STATIC_LIB ON  CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_EXAMPLES   OFF CACHE BOOL "" FORCE)

# Make necessary targets available

FetchContent_MakeAvailable(
    # Base libraries
    VulkanHeaders
    VulkanLoader

    # SPIRV manipulation libraries
    SPIRVHeaders
    SPIRVTools
    SPIRVCross
    SPIRVReflect

    # Shader compilers
    glslang
    shaderc

    # Other
    #volk
    #VMA
)

add_library(Vulkan::Loader ALIAS vulkan)

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

add_library(SPIRV-Reflect ALIAS spirv-reflect-static)

