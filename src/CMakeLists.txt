cmake_minimum_required(VERSION 3.16.0)

project(source)

message(STATUS "Compiling to run locally")

file(GLOB SRC "*/*.*" "*/*/*.*")
add_library(source ${SRC})

target_compile_features(source PRIVATE cxx_std_17)

target_include_directories(source PRIVATE *)
target_include_directories(source PRIVATE ../include/)
target_include_directories(source PRIVATE ../include/impl/unix/)
