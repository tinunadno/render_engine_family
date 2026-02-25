cmake_minimum_required(VERSION 3.20)
project(sfm_pipeline LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)

include(FetchContent)

# ---------------- Eigen ----------------
# Сначала ищем в системе (быстро)
find_package(Eigen3 3.4 QUIET)

if(NOT Eigen3_FOUND)
    message(STATUS "Eigen3 not found in system, downloading...")
    FetchContent_Declare(
            eigen
            GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
            GIT_TAG 3.4.0
    )
    FetchContent_MakeAvailable(eigen)
else()
    message(STATUS "Found system Eigen3: ${Eigen3_DIR}")
endif()

# ---------------- COLMAP ----------------
# Отключаем лишнее ДО FetchContent
set(COLMAP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(COLMAP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(COLMAP_BUILD_GUI OFF CACHE BOOL "" FORCE)
set(COLMAP_CUDA_ENABLED ON CACHE BOOL "" FORCE)

# Dockerfile подменит это локальной папкой через FETCHCONTENT_SOURCE_DIR_COLMAP
FetchContent_Declare(
        colmap
        GIT_REPOSITORY https://github.com/colmap/colmap.git
        GIT_TAG main 
)
# Версии 3.13.0 не существует, лучше указывать main или хеш, 
# но Dockerfile всё равно это переопределит.

FetchContent_MakeAvailable(colmap)

find_package(OpenMP REQUIRED)

add_executable(sfm_pipeline
        test.cpp
)

target_compile_definitions(sfm_pipeline
        PRIVATE PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(sfm_pipeline
        PRIVATE
        /deps/colmap/src
        ${colmap_SOURCE_DIR}/src
        ${colmap_BINARY_DIR}
        ${colmap_BINARY_DIR}/src
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