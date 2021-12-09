cmake_minimum_required(VERSION 3.20)
project(IT517G)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") # Statically link MSVC

add_executable(game src/main.cpp src/motor/logging.hpp src/motor/libs/glad.h src/motor/libs/implementations.cpp src/motor/File.cpp src/motor/File.hpp src/motor/Application.cpp src/motor/Application.hpp src/motor/Application_win.cpp src/motor/Input.cpp src/motor/Input.hpp src/motor/Rendering.cpp src/motor/Rendering.hpp src/motor/Draw.hpp src/motor/typedefs.hpp src/motor/Mem.cpp src/motor/Mem.hpp  "src/motor/Time.hpp" "src/motor/Entity.hpp" "src/game/Player.hpp" "src/game/Player.cpp" "src/motor/Math.hpp")

target_include_directories(game PRIVATE src)

set(game PRIVATE WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
set_property(TARGET game PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
set_property(TARGET game PROPERTY RULE_LAUNCH_COMPILE  "\"${CMAKE_COMMAND}\" -E time") # Print elapsed time when compiling

# This makes it possible for logging function to print relative filename
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_LENGTH)
target_compile_definitions(game PRIVATE "-DSOURCE_PATH_LENGTH=${SOURCE_PATH_LENGTH}")

set(SPRITE_SHEET "${CMAKE_CURRENT_SOURCE_DIR}/data/atlas.png")
set(SPRITE_DATA "${CMAKE_CURRENT_SOURCE_DIR}/data/atlas.json")
set(ASEPRITE_EXECUTABLE CACHE FILEPATH "Path to aseprite.exe")

add_custom_target(generate_sprite_sheet ALL
	COMMAND "${ASEPRITE_EXECUTABLE}" -b --filename-format "{title}.{tag}#{tagframe000}" ${CMAKE_CURRENT_SOURCE_DIR}/assets/sprites/* --data ${SPRITE_DATA} --format json-array --sheet ${SPRITE_SHEET} --sheet-pack --sheet-width 1024 --sheet-height 1024)

if (MSVC AND CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_options(game PRIVATE "/ZI")
	target_link_options(game PRIVATE "/ZI" "/INCREMENTAL")
endif()

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)
target_compile_definitions(game PRIVATE GLFW_INCLUDE_NONE) # Ensures we don't have to worry about include order of GLFW and GLAD
add_subdirectory(deps/glfw)
target_link_libraries(game PRIVATE glfw)

# RapidJSON
target_include_directories(game PRIVATE deps/rapidjson/include)

