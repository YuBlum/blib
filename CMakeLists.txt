cmake_minimum_required(VERSION 3.27.1)

project(BLIB)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/glfw/)

add_library(blib ./src/blib.c)
target_include_directories(blib PUBLIC ./external/glfw/include/)
target_link_directories(blib PRIVATE external/glfw/src)
target_link_libraries(blib glfw)
target_compile_options(blib PRIVATE -std=c99 -pedantic -Werror -Wall -Wextra)

add_executable(example ./examples/example.c)
target_include_directories(example PUBLIC ./src/)
target_link_libraries(example blib)
