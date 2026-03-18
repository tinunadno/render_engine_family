cmake_minimum_required(VERSION 3.15)

project(text_io LANGUAGES CXX)

add_library(text_io INTERFACE
        include/text_io/tio.h)

target_include_directories(text_io
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(text_io INTERFACE cxx_std_20)

target_compile_definitions(text_io
        INTERFACE
)

