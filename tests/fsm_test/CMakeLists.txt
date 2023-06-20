cmake_minimum_required(VERSION 3.16.0)

project(libriccore_fsm_test)

add_compile_options(-g)
add_compile_options(-O0)
add_compile_options(-Wall)
add_compile_options(-Wpedantic)


set(LOCAL ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../.. ${CMAKE_CURRENT_SOURCE_DIR}/../../build)

# #hack to get librnp to compile properly, need to convert them to cmake libraries 
# file(GLOB_RECURSE LIBRNP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/librnp/src/*.cpp)

# message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}../../)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/librnp/ ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/librnp/bin)


# add_executable(libriccore_fsm_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${LIBRNP_SRC})
add_executable(libriccore_fsm_test ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

target_compile_features(libriccore_fsm_test PRIVATE cxx_std_17)
target_include_directories(libriccore_fsm_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libriccore_fsm_test PRIVATE libriccore)
target_link_libraries(libriccore_fsm_test PRIVATE librnp)


# target_include_directories(libriccore_fsm_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/librnp/src)
