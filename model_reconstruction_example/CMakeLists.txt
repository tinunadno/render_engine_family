cmake_minimum_required(VERSION 3.15)

project(model_reconstruction_example LANGUAGES CXX)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(model_reconstruction_example
        main.cpp
        snapshot.h
)

target_link_libraries(model_reconstruction_example
        PRIVATE
        shader_core
        model_render_core
        glfw
        OpenGL::GL
)
