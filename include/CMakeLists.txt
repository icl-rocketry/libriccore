cmake_minimum_required(VERSION 3.16.0)

project(hal)

# Which mode to compile in
option(LOCAL "Pick the ESP32 or DESKTOP (default)" ON)

if (LOCAL)
    message(STATUS "Compiling to run locally")

    file(GLOB SRC "impl/unix/*.cpp")
    add_library(hal ${SRC})

    target_compile_features(hal PRIVATE cxx_std_17)
    target_include_directories(hal PRIVATE .)
    target_include_directories(hal PRIVATE impl/unix/)

    # Add pthreads
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(hal PRIVATE Threads::Threads)
    
else()
    message(STATUS "TODO implement me")
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
endif()

