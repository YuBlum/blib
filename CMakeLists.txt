cmake_minimum_required(VERSION 3.27.1)

project(BLIB)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(blib ./src/blib.c)

add_executable(example ./examples/example.c)
target_include_directories(example PRIVATE ./src/)
target_link_directories(example PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(example blib)
