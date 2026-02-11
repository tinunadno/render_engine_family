cmake_minimum_required(VERSION 3.15)

if(APPLE)
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")
    include_directories("/opt/homebrew/opt/libomp/include")
endif()

project(sfm_model_reconstruction LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(colmap REQUIRED)

add_executable(sfm_model_reconstruction
        test.cpp
        snapshot.h
        sfm/run_sfm.h
        test.cpp
)

target_compile_definitions(sfm_model_reconstruction PRIVATE PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(sfm_model_reconstruction
        PRIVATE
        engine_core
        model_render_core
        glfw
        OpenGL::GL
        colmap::colmap_controllers
        colmap::colmap_sfm
        colmap::colmap_scene
        colmap::colmap_feature
        colmap::colmap_estimators
        colmap::colmap_util
        Eigen3::Eigen
)
