cmake_minimum_required(VERSION 3.15)

project(black_hole_shader LANGUAGES CXX)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(black_hole_shader
        main.cpp
        ../sfm_model_reconstruction/main.cpp
        ../sfm_model_reconstruction/snapshot.h
        ../particles_exmaple/main.cpp
)

target_link_libraries(black_hole_shader
        PRIVATE
        engine_core
        ray_marching_core
        glfw
        OpenGL::GL
)
