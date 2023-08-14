cmake_minimum_required(VERSION 3.19)

add_library(unix INTERFACE)
#some specific compiler requirements
<<<<<<< HEAD
target_compile_features(unix INTERFACE cxx_std_17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(unix INTERFACE Threads::Threads)
=======
target_compile_features(unix PRIVATE cxx_std_17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(unix PRIVATE Threads::Threads)
>>>>>>> 8b3b7fb (restructured how platform specific stubs will be included into library. look at the millis stub for an example)

#add sources of interface library here
file(GLOB SRC "*.cpp")
target_sources(unix INTERFACE ${SRC})