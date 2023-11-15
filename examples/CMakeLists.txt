cmake_minimum_required(VERSION 3.0...3.25)
project(
    00-hello-triangle
    VERSION 0.1.0
    LANGUAGES CXX C
)

add_executable(00-hello-triangle 00-hello-triangle/main.cpp)

#add_subdirectory(3rdparty/glfw)

target_include_directories(OctoGFX PUBLIC ../include)
target_include_directories(OctoGFX PUBLIC 3rdparty/glfw/include)

target_link_libraries(00-hello-triangle PRIVATE OctoGFX)
target_link_libraries(00-hello-triangle PRIVATE glfw)

set_target_properties(00-hello-triangle PROPERTIES
    CXX_STANDARD 20
    COMPILE_WARNING_AS_ERROR ON
)

if (MSVC)
    target_compile_options(00-hello-triangle PRIVATE /W4)
else()
    target_compile_options(00-hello-triangle PRIVATE -Wall -Wextra -pedantic)
endif()

if(XCODE)
    set_target_properties(00-hello-triangle PROPERTIES
        XCODE_GENERATE_SCHEME ON
        XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal")
endif()