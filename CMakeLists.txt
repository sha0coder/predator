cmake_minimum_required(VERSION 3.5)
project(predator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
include_directories("/usr/include/libr/")
link_directories("/usr/lib/")


#find_package(libr REQUIRED)

add_executable(predator
        include/genotype.hpp
        include/eval.hpp
        include/population.hpp
        include/sandbox.hpp
        include/r2.hpp
        eval.cpp
        genotype.cpp
        population.cpp
        predator.cpp
        sandbox.cpp
        r2.cpp)


target_link_libraries(predator "r_socket")


