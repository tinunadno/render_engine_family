cmake_minimum_required(VERSION 3.15)

project(text_io_example LANGUAGES CXX)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(text_io_example
        main.cpp
)

target_link_libraries(text_io_example
        PRIVATE
        stb
        engine_core
        text_render_lib
        text_io
        glfw
        OpenGL::GL
)
