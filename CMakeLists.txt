cmake_minimum_required(VERSION 3.27.1)

project(BLIB)

add_subdirectory(external/glfw/)

add_library(devember SHARED ./examples/devember/snake.c)
target_include_directories(devember PUBLIC ./src/)

add_library(glad STATIC ./vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC ./vendor/glad/include/)

add_library(stb_image STATIC ./vendor/stb_image/stb_image.c)
target_include_directories(stb_image PUBLIC ./vendor/stb_image/)

add_executable(blib ./src/blib.c)
target_include_directories(blib PUBLIC ./external/glfw/include/ ./vendor/glad/include/)
target_link_directories(blib PRIVATE external/glfw/src)
target_link_libraries(blib glfw uuid devember glad stb_image)
target_compile_options(blib PRIVATE -std=c99 -pedantic -Werror -Wall -Wextra)

# add_executable(example ./examples/example.c)
# target_include_directories(example PUBLIC ./src/)
# target_link_libraries(example blib)
