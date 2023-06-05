include(FetchContent)

set(OPENJDK_URL "")
set(OPENJDK_SHA256 "")
if(WIN32)
    set(OPENJDK_URL "https://download.java.net/java/GA/jdk20.0.1/b4887098932d415489976708ad6d1a4b/9/GPL/openjdk-20.0.1_windows-x64_bin.zip")
    set(OPENJDK_SHA256 "31ca4a8cbdea1da7fb441194e756dd1adbedfc05bd1135a1ecc46b4288ea8942")
elseif(APPLE)
    set(OPENJDK_URL "https://download.java.net/java/GA/jdk20.0.1/b4887098932d415489976708ad6d1a4b/9/GPL/openjdk-20.0.1_macos-aarch64_bin.tar.gz")
    set(OPENJDK_SHA256 "78ae5bb4c96632df8d3f776919c95653d1afd3e715981c4d33be5b3c81d05420")
else()
    set(OPENJDK_URL "https://download.java.net/java/GA/jdk20.0.1/b4887098932d415489976708ad6d1a4b/9/GPL/openjdk-20.0.1_linux-x64_bin.tar.gz")
    set(OPENJDK_SHA256 "4248a3af4602dbe2aefdb7010bc9086bf34a4155888e837649c90ff6d8e8cef9")
endif()

FetchContent_Declare(
    openjdk
    URL      ${OPENJDK_URL}
    URL_HASH SHA256=${OPENJDK_SHA256}
)

FetchContent_GetProperties(openjdk)
if(NOT openjdk_POPULATED)
    FetchContent_Populate(openjdk)
    add_library(openjdk SHARED IMPORTED GLOBAL)
    
    if (WIN32)
        set(OPENJDK_JVM_DIR ${openjdk_SOURCE_DIR}/bin/server)
        set(OPENJDK_JVM_LIB ${OPENJDK_JVM_DIR}/jvm.dll)
        target_include_directories(openjdk
            INTERFACE
            ${openjdk_SOURCE_DIR}/include
            ${openjdk_SOURCE_DIR}/include/win32
            ${openjdk_SOURCE_DIR}/include/win32/bridge
        )
        set_target_properties(openjdk PROPERTIES
            IMPORTED_IMPLIB ${openjdk_SOURCE_DIR}/lib/jvm.lib
        )
    elseif(APPLE)
        set(OPENJDK_JVM_DIR ${openjdk_SOURCE_DIR}/lib/server)
        set(OPENJDK_JVM_LIB ${OPENJDK_JVM_DIR}/libjvm.dylib)
        target_include_directories(openjdk
            INTERFACE
            ${openjdk_SOURCE_DIR}/include
            ${openjdk_SOURCE_DIR}/include/darwin
        )
    else()
        set(OPENJDK_JVM_DIR ${openjdk_SOURCE_DIR}/lib/server)
        set(OPENJDK_JVM_LIB ${OPENJDK_JVM_DIR}/libjvm.so)
        target_include_directories(openjdk
            INTERFACE
            ${openjdk_SOURCE_DIR}/include
            ${openjdk_SOURCE_DIR}/include/linux
        )
    endif()

    set_target_properties(openjdk PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "C"
        IMPORTED_LOCATION ${openjdk_SOURCE_DIR}
    )
endif()
