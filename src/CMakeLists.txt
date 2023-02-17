cmake_minimum_required(VERSION 3.16.0)

message(STATUS "Compiling to run locally")

file(GLOB_RECURSE SRC "*.h" "*.cpp")
add_library(source ${SRC})

set_target_properties(source PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(source PRIVATE cxx_std_17)

target_include_directories(source PRIVATE *)
target_include_directories(source PRIVATE ../include/)
target_include_directories(source PRIVATE ../include/impl/unix/)
target_include_directories(source PRIVATE libriccore/)

