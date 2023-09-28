cmake_minimum_required(VERSION 3.16)

#define interface library so we can add sources into libriccore
add_library(storage INTERFACE)

#glob cos we are lazy 
file(GLOB SRC "*.cpp")
target_sources(storage INTERFACE ${SRC})
