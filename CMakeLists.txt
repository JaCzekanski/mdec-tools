cmake_minimum_required(VERSION 3.17)
project(mdec-tools)

set(CMAKE_CXX_STANDARD 17)

# stb_image
add_library(stb_image STATIC
        externals/stb/stb_image.c
        )
target_include_directories(stb_image PUBLIC externals/stb)


add_executable(mdec-tools
        src/main.cpp
        src/types.cpp
        src/rle.cpp
        src/quantize.cpp
        src/quant_tables.cpp
        )

target_include_directories(mdec-tools
        PUBLIC
        src
        )
target_link_libraries(mdec-tools
        stb_image)
