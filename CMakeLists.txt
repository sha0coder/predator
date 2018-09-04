cmake_minimum_required(VERSION 3.10)
project(predator)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
include_directories("/usr/include/libr/")
link_directories("/usr/lib/")


#find_package(libr REQUIRED)

add_executable(predator
        include/eval.hpp
        include/genotype.hpp
        include/population.hpp
        include/sandbox.hpp
        eval.cpp
        genotype.cpp
        population.cpp
        predator.cpp
        sandbox.cpp)


target_link_libraries(predator "r_socket r_util")


