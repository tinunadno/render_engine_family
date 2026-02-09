cmake_minimum_required(VERSION 3.15)

project(texture_example LANGUAGES CXX)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(texture_example
        main.cpp
)

target_link_libraries(texture_example
        PRIVATE
        shader_core
        model_render_core
        glfw
        OpenGL::GL
)
