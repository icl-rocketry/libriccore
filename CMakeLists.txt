cmake_minimum_required(VERSION 3.16.0)

project("librcicore" VERSION 1.0)

add_library(libriccore INTERFACE)

target_compile_definitions(libriccore INTERFACE -g)
target_compile_definitions(libriccore INTERFACE -Wall)
target_compile_definitions(libriccore INTERFACE -Wpedantic)
target_compile_definitions(libriccore INTERFACE -O0)
target_compile_features(libriccore INTERFACE cxx_std_17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_link_libraries(libriccore INTERFACE source)

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
