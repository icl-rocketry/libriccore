cmake_minimum_required(VERSION 3.16)

add_library(drivers INTERFACE)

# add each driver family as a subdirectory so nested sources are linked in
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/adc)
target_link_libraries(drivers INTERFACE adc)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/power)
target_link_libraries(drivers INTERFACE power)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/sensors)
target_link_libraries(drivers INTERFACE sensors)
