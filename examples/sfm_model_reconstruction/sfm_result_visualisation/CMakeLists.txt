cmake_minimum_required(VERSION 3.15)

project(sfm_result_visualisation LANGUAGES CXX)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(sfm_result_visualisation
        main.cpp
)

target_compile_definitions(sfm_result_visualisation PRIVATE PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(sfm_result_visualisation
        PRIVATE
        engine_core
        model_render_core
        glfw
        OpenGL::GL
)
