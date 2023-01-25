cmake_minimum_required(VERSION 3.16.0)

project(libriccore)
message(STATUS "compiling main")

add_compile_options(-g)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/libriccore/)

add_executable(libriccore src/main.cpp)
target_compile_features(libriccore PRIVATE cxx_std_17)

target_link_libraries(libriccore hal source)

target_include_directories(libriccore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_include_directories(libriccore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/libriccore)
target_include_directories(libriccore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(libriccore PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/impl/unix/)
