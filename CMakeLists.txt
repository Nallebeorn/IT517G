cmake_minimum_required(VERSION 3.20)
project(IT517G)

set(CMAKE_CXX_STANDARD 14)


add_executable(game src/main.cpp src/logging.hpp)

string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_DIR_PATH_LENGTH)
target_compile_definitions(game PRIVATE SOURCE_DIR_PATH_LENGTH=${SOURCE_DIR_PATH_LENGTH})

add_subdirectory(deps/fmt)

target_link_libraries(game PRIVATE fmt)