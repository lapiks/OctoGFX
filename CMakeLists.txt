cmake_minimum_required(VERSION 3.0...3.25)
project(
    OctoEngine
    VERSION 0.1.0
    LANGUAGES CXX C
)

add_executable(OctoEngine src/main.cpp)

add_subdirectory(3rdparty/glfw)
add_subdirectory(3rdparty/WebGPU-distribution)
add_subdirectory(3rdparty/glfw3webgpu)
target_link_libraries(OctoEngine PRIVATE glfw webgpu glfw3webgpu)

set_target_properties(OctoEngine PROPERTIES
    CXX_STANDARD 20
    COMPILE_WARNING_AS_ERROR ON
)

if (MSVC)
    target_compile_options(OctoEngine PRIVATE /W4)
else()
    target_compile_options(OctoEngine PRIVATE -Wall -Wextra -pedantic)
endif()

if(XCODE)
    set_target_properties(OctoEngine PROPERTIES
        XCODE_GENERATE_SCHEME ON
        XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
endif()