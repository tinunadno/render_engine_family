cmake_minimum_required(VERSION 3.15)

project(text_render_lib LANGUAGES CXX)

add_library(text_render_lib INTERFACE)

target_include_directories(text_render_lib
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(text_render_lib INTERFACE cxx_std_20)

target_compile_definitions(text_render_lib
        INTERFACE
)

