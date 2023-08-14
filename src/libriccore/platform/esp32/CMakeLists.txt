<<<<<<< HEAD
cmake_minimum_required(VERSION 3.16)
=======
cmake_minimum_required(VERSION 3.19)
>>>>>>> 8b3b7fb (restructured how platform specific stubs will be included into library. look at the millis stub for an example)

add_library(esp32 INTERFACE)

#add sources of interface library here
<<<<<<< HEAD
#worth noting that dependancies such as the freertos threading library
#is dealt with the component level config in the IDF so we dont need to 
#explictly link here

=======
>>>>>>> 8b3b7fb (restructured how platform specific stubs will be included into library. look at the millis stub for an example)
file(GLOB SRC "*.cpp")
target_sources(esp32 INTERFACE ${SRC})