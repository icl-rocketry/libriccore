cmake_minimum_required(VERSION 3.16.0)

project(virutalserialport_test)

add_compile_options(-g)
add_compile_options(-O0)
add_compile_options(-Wall)
add_compile_options(-Wpedantic)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_SOURCE_DIR}/../../build)


add_executable(virutalserialport_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_compile_features(virutalserialport_test PRIVATE cxx_std_17)

target_link_libraries(virutalserialport_test PRIVATE libriccore)
