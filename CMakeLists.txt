cmake_minimum_required(VERSION 3.16.0)

project(libriccore)

add_compile_options(-g)

add_subdirectory(include)
add_subdirectory(src)

add_executable(libriccore test/main.cpp)
target_compile_features(libriccore PRIVATE cxx_std_17)

target_link_libraries(libriccore hal source)

target_include_directories(libriccore PRIVATE src/)
target_include_directories(libriccore PRIVATE include/)
target_include_directories(libriccore PRIVATE include/impl/unix/)
