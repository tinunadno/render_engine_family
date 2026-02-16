cmake_minimum_required(VERSION 3.15)

project(particle_example LANGUAGES CXX)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(particle_example
        main.cpp
)

target_compile_definitions(particle_example PRIVATE PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(particle_example
        PRIVATE
        engine_core
        model_render_core
        glfw
        OpenGL::GL
)
