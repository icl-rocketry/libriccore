cmake_minimum_required(VERSION 3.16.0)

if (ESP_PLATFORM)
    set(LOCAL OFF)
    idf_component_register(INCLUDE_DIRS src
                           REQUIRES librnp
                           SRC_DIRS src/libriccore/platform src/libriccore/platform/esp32 src/libriccore/storage)
    #link any ESP-IDF required libraries here
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)
    # include_directories(${CMAKE_SOURCE_DIR}/src/Config)
    target_include_directories(${COMPONENT_LIB} INTERFACE ${CMAKE_SOURCE_DIR}/src) #required to allow config tweaks to work
    target_link_libraries(${COMPONENT_LIB} INTERFACE libriccore)

    return()
else()
    option(LOCAL "Pick the ESP32 or DESKTOP (default)" ON)
endif()

project("libriccore" VERSION 1.0)

#include src
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/)
