cmake_minimum_required(VERSION 3.16.0)

add_library(libriccore INTERFACE)

if (LOCAL)
    target_compile_options(libriccore INTERFACE -Wall)
    target_compile_options(libriccore INTERFACE -Wpedantic)
    target_compile_features(libriccore INTERFACE cxx_std_17)
endif()

target_include_directories(libriccore INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# set_target_properties(libriccore PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libriccore/platform)
target_link_libraries(libriccore INTERFACE platform)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libriccore/storage)
target_link_libraries(libriccore INTERFACE storage)

