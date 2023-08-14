<<<<<<< HEAD
cmake_minimum_required(VERSION 3.16)
=======
cmake_minimum_required(VERSION 3.19)
>>>>>>> 8b3b7fb (restructured how platform specific stubs will be included into library. look at the millis stub for an example)

add_library(platform INTERFACE)

 #link correct sources into library
 #we dont need to include these directories as they are already included above
if (LOCAL)
<<<<<<< HEAD
    message(STATUS "compiling for UNIX")
=======
>>>>>>> 8b3b7fb (restructured how platform specific stubs will be included into library. look at the millis stub for an example)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unix)
    target_link_libraries(platform INTERFACE unix)
    target_compile_definitions(platform INTERFACE LIBRICCORE_UNIX)
else()
<<<<<<< HEAD
    message(STATUS "compiling for ESP32")
=======
>>>>>>> 8b3b7fb (restructured how platform specific stubs will be included into library. look at the millis stub for an example)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/esp32)
    target_link_libraries(platform INTERFACE esp32)
endif()