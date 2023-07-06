cmake_minimum_required(VERSION 3.16.0)

project(threading_test)

add_compile_options(-g)
add_compile_options(-O0)
add_compile_options(-Wall)
add_compile_options(-Wpedantic)


set(LOCAL ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_SOURCE_DIR}/../../build)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/librnp/ ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/librnp/bin)

add_executable(threading_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_compile_features(threading_test PRIVATE cxx_std_17)
target_include_directories(threading_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(threading_test PRIVATE libriccore)
target_link_libraries(threading_test PRIVATE librnp)


