cmake_minimum_required(VERSION 3.0...3.25)
project(
    OctoGFX
    VERSION 0.1.0
    LANGUAGES CXX C
)

add_library(OctoGFX STATIC src/octogfx.cpp src/renderer_context.cpp)

add_subdirectory(examples)
add_subdirectory(3rdparty/WebGPU-distribution)

target_include_directories(OctoGFX PRIVATE include)
target_link_libraries(OctoGFX PRIVATE webgpu)

source_group(headers include/octogfx.h)

set_target_properties(OctoGFX PROPERTIES
    CXX_STANDARD 11
    COMPILE_WARNING_AS_ERROR ON
)

if (MSVC)
    target_compile_options(OctoGFX PRIVATE /W4)
else()
    target_compile_options(OctoGFX PRIVATE -Wall -Wextra -pedantic)
endif()

if(XCODE)
    set_target_properties(OctoGFX PROPERTIES
        XCODE_GENERATE_SCHEME ON
        XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
endif()