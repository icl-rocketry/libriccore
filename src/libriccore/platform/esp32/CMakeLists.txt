cmake_minimum_required(VERSION 3.19)

add_library(esp32 INTERFACE)

#add sources of interface library here
file(GLOB SRC "*.cpp")
target_sources(esp32 INTERFACE ${SRC})