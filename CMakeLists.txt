#cmake_minimum_required(VERSION 3.1)
##
##set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
##set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#
#project(threads VERSION 1.0 LANGUAGES C CXX)
#
#add_library(Ex2 uthreads.h uthreads.cpp Thread.cpp Thread.h jona1.cpp)

#set_property(TARGET uthreads PROPERTY CXX_STANDARD 11)
#target_compile_options(uthreads PUBLIC -Wall -Wextra)



#add_subdirectory(tests)

cmake_minimum_required(VERSION 3.15)
project(Ex2)

set(CMAKE_CXX_STANDARD 14)

add_executable(Ex2 uthreads.cpp uthreads.h Thread.h Thread.cpp jona1.cpp)