# Oversimplified host system detector mostly for recognizing Windows, Linux
# and macOS. Probably unnecessary, since it's just aliasing variables that
# CMake provides by default, but it gives me more consistency.

if(WIN32)
    set(PLATFORM_WINDOWS 1)
elseif(APPLE)
    set(PLATFORM_MACOS 1)
elseif(LINUX)
    set(PLATFORM_LINUX 1)
endif()
