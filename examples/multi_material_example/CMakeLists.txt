cmake_minimum_required(VERSION 3.15)

project(multi_material_example LANGUAGES CXX)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(multi_material_example
        main.cpp
)

target_compile_definitions(multi_material_example PRIVATE PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(multi_material_example
        PRIVATE
        engine_core
        model_render_core
        glfw
        OpenGL::GL
)

add_executable(heart_example
        heart_example.cpp
)

target_compile_definitions(heart_example PRIVATE PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(heart_example
        PRIVATE
        engine_core
        model_render_core
        glfw
        OpenGL::GL
)
