cmake_minimum_required(VERSION 3.16)

add_library(esp32 INTERFACE)

#add sources of interface library here
#worth noting that dependancies such as the freertos threading library
#is dealt with the component level config in the IDF so we dont need to 
#explictly link here

file(GLOB SRC "*.cpp")
target_sources(esp32 INTERFACE ${SRC})