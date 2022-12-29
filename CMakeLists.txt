cmake_minimum_required(VERSION 3.16.0)

project(libriccore)

# Which mode to compile in
option(LOCAL "Pick the ESP32 or DESKTOP (default)" ON)

add_executable(libriccore src/main.cpp include/impl/unix/thread.cpp)
if (LOCAL)
    message(STATUS "Compiling to run locally")

    # Compiler options
    target_include_directories(libriccore PRIVATE "./include/" "./include/impl/unix/")
    target_compile_features(libriccore PRIVATE cxx_std_17)
    target_compile_options(libriccore PRIVATE -Wall -Wextra -Wpedantic)

    # Add pthreads
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(libriccore PRIVATE Threads::Threads)


    
elseif (PLATFORM STREQUAL ESP32)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
endif()

