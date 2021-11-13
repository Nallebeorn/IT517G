cmake_minimum_required(VERSION 3.20)
project(IT517G LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

add_executable(game src/main.cpp src/logging.hpp src/two.cpp include/glad/glad.h src/implementations.cpp)

target_include_directories(game PRIVATE include)

# Print elapsed time when compiling
set_property(TARGET game PROPERTY RULE_LAUNCH_COMPILE "\"${CMAKE_COMMAND}\" -E time")

add_subdirectory(deps/fmt)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)

target_link_libraries(game PRIVATE fmt glfw)

target_precompile_headers(game PRIVATE <fmt/core.h> <fmt/chrono.h>)
