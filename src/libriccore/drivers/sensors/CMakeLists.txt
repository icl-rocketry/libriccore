cmake_minimum_required(VERSION 3.16)

add_library(sensors INTERFACE)

file(GLOB SRC "*.cpp")
target_sources(sensors INTERFACE ${SRC})
