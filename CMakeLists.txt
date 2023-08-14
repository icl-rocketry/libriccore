cmake_minimum_required(VERSION 3.16.0)

if (ESP_PLATFORM)
    set(LOCAL OFF)
    idf_component_register(INCLUDE_DIRS src
                           REQUIRES librnp)
    #link any ESP-IDF required libraries here
    return()
else()
    option(LOCAL "Pick the ESP32 or DESKTOP (default)" ON)
endif()

project("libriccore" VERSION 1.0)

<<<<<<< HEAD
#include src
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)
=======
target_compile_definitions(libriccore INTERFACE -g)
target_compile_definitions(libriccore INTERFACE -Wall)
target_compile_definitions(libriccore INTERFACE -Wpedantic)
target_compile_definitions(libriccore INTERFACE -O0)
target_compile_features(libriccore INTERFACE cxx_std_17)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_link_libraries(libriccore INTERFACE source)

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)
option(LOCAL "Pick the ESP32 or DESKTOP (default)" ON)
>>>>>>> 8b3b7fb (restructured how platform specific stubs will be included into library. look at the millis stub for an example)
