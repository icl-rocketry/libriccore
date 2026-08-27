cmake_minimum_required(VERSION 3.16)

add_library(power INTERFACE)

file(GLOB SRC "*.cpp")
target_sources(power INTERFACE ${SRC})
