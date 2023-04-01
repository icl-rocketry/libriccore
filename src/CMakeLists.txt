cmake_minimum_required(VERSION 3.16.0)

message(STATUS "Compiling libriccore sources")

add_library(source INTERFACE)

target_include_directories(source INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(source PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libriccore/platform)
target_link_libraries(source INTERFACE platform)

