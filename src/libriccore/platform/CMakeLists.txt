cmake_minimum_required(VERSION 3.16)

add_library(platform INTERFACE)

 #link correct sources into library
 #we dont need to include these directories as they are already included above
if (LOCAL)
    message(STATUS "compiling for UNIX")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unix)
    target_link_libraries(platform INTERFACE unix)
    target_compile_definitions(platform INTERFACE LIBRICCORE_UNIX)
else()
    message(STATUS "compiling for ESP32")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/esp32)
    target_link_libraries(platform INTERFACE esp32)
endif()