cmake_minimum_required(VERSION 3.14)
project(bw-pecs
        VERSION 0.1
        LANGUAGES CXX)

add_library(pecs INTERFACE)
add_library(bw::pecs ALIAS pecs)

target_include_directories(pecs INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(pecs INTERFACE cxx_std_20)