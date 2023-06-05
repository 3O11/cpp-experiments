include(${CMAKE_SOURCE_DIR}/cmake/openjdk.cmake)

add_executable(20-BundleJava
    "main.cpp"
)

set_property(TARGET 20-BundleJava PROPERTY CXX_STANDARD 20)

target_link_libraries(20-BundleJava
    PRIVATE
    openjdk
)
