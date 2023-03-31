cmake_minimum_required(VERSION 3.16.0)

message(STATUS "Compiling libriccore sources")

add_library(source INTERFACE)

target_include_directories(source INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(source PROPERTIES LINKER_LANGUAGE CXX)


