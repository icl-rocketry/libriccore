cmake_minimum_required(VERSION 3.19)

add_library(unix INTERFACE)
#some specific compiler requirements
target_compile_features(unix PRIVATE cxx_std_17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(unix PRIVATE Threads::Threads)

#add sources of interface library here
file(GLOB SRC "*.cpp")
target_sources(unix INTERFACE ${SRC})