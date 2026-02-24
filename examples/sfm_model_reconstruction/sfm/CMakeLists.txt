cmake_minimum_required(VERSION 3.20)
project(sfm_pipeline LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------------- Eigen ----------------
FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
)
FetchContent_MakeAvailable(eigen)

# ---------------- COLMAP ----------------
set(COLMAP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(COLMAP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(COLMAP_BUILD_GUI OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        colmap
        GIT_REPOSITORY https://github.com/colmap/colmap.git
        GIT_TAG 3.13
)
FetchContent_MakeAvailable(colmap)

find_package(OpenMP REQUIRED)

add_executable(sfm_pipeline
        test.cpp
)

target_compile_definitions(sfm_pipeline
        PRIVATE PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(sfm_pipeline
        PRIVATE
        colmap_controllers
        colmap_sfm
        colmap_scene
        colmap_feature
        colmap_estimators
        colmap_util
        Eigen3::Eigen
        OpenMP::OpenMP_CXX
)