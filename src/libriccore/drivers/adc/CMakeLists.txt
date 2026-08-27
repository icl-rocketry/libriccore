cmake_minimum_required(VERSION 3.16)

add_library(adc INTERFACE)

file(GLOB SRC "*.cpp")
target_sources(adc INTERFACE ${SRC})
