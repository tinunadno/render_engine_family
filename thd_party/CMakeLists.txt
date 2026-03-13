cmake_minimum_required(VERSION 3.15)

project(stb LANGUAGES CXX)

add_library(stb INTERFACE)

target_include_directories(stb
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb>
        $<INSTALL_INTERFACE:stb>
)

target_compile_features(stb INTERFACE cxx_std_20)

target_compile_definitions(stb
        INTERFACE
)
