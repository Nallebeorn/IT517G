cmake_minimum_required(VERSION 3.20)
project(IT517G)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # Statically link MSVC

add_executable(game src/main.cpp src/motor/logging.hpp src/motor/libs/glad.h src/motor/libs/implementations.cpp src/motor/File.cpp src/motor/File.hpp src/motor/Application.cpp src/motor/Application.hpp src/motor/Application_win.cpp src/motor/Input.cpp src/motor/Input.hpp src/motor/Rendering.cpp src/motor/Rendering.hpp src/motor/Draw.hpp src/motor/typedefs.hpp src/motor/Mem.cpp src/motor/Mem.hpp )

target_include_directories(game PRIVATE include src)

set(game PRIVATE WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
set_property(TARGET game PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
set_property(TARGET game PROPERTY RULE_LAUNCH_COMPILE  "\"${CMAKE_COMMAND}\" -E time") # Print elapsed time when compiling

if (MSVC AND CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_options(game PRIVATE "/ZI")
	target_link_options(game PRIVATE "/ZI" "/INCREMENTAL")
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)
target_compile_definitions(game PRIVATE GLFW_INCLUDE_NONE) # Ensures we don't have to worry about include order of GLFW and GLAD
add_subdirectory(deps/glfw)

target_link_libraries(game PRIVATE glfw)
