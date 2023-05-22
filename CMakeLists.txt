cmake_minimum_required(VERSION 3.16.0)

if (ESP_PLATFORM)
    set(LOCAL OFF)
    idf_component_register()
    #link any ESP-IDF required libraries here
else()
    option(LOCAL "Pick the ESP32 or DESKTOP (default)" ON)
endif()

project("libriccore" VERSION 1.0)

#include src
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)