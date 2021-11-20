cmake_minimum_required(VERSION 3.20)
project(IT517G)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # Statically link MSVC

set(SOURCES src/main.cpp src/logging.hpp include/glad/glad.h src/implementations.cpp src/File.cpp src/File.hpp src/Application.cpp src/Application.hpp src/Application_win.cpp src/Input.cpp src/Input.hpp src/Rendering.cpp src/Rendering.hpp)
add_executable(game ${SOURCES})

target_include_directories(game PRIVATE include src)

set(game PRIVATE WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

# Print elapsed time when compiling
set_property(TARGET game PROPERTY RULE_LAUNCH_COMPILE  "\"${CMAKE_COMMAND}\" -E time")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)
target_compile_definitions(game PRIVATE GLFW_INCLUDE_NONE) # Ensures we don't have to worry about include order of GLFW and GLAD
add_subdirectory(deps/glfw)

target_link_libraries(game PRIVATE glfw)
