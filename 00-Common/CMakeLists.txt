include(${CMAKE_SOURCE_DIR}/cmake/mathlib.cmake)

add_library(CommonUtils INTERFACE)

target_include_directories(CommonUtils
    INTERFACE
    Utils
)

target_link_libraries(CommonUtils
    INTERFACE
    MathLib
)

set_target_properties(CommonUtils
    PROPERTIES
    CXX_STANDARD 20
)
