include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/3O11/stb
)

FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)

    FetchContent_Populate(stb)

    file(WRITE ${stb_SOURCE_DIR}/stb_image.c
        "#define STB_IMAGE_IMPLEMENTATION \n"
        "#include \"stb_image.h\"\n"
    )

    add_library(stb_image STATIC
        ${stb_SOURCE_DIR}/stb_image.h
        ${stb_SOURCE_DIR}/stb_image.c
    )

    target_include_directories(stb_image
        PUBLIC
        ${stb_SOURCE_DIR}
    )

    file(WRITE ${stb_SOURCE_DIR}/stb_image_write.c
        "#define STB_IMAGE_WRITE_IMPLEMENTATION"
        "#include \"stb_image_write.h\"\n"
    )

    add_library(stb_image_write STATIC
        ${stb_SOURCE_DIR}/stb_image_write.h
        ${stb_SOURCE_DIR}/stb_image_write.c
    )

    target_include_directories(stb_image_write
        PUBLIC
        ${stb_SOURCE_DIR}
    )

endif()
