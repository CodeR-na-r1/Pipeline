cmake_minimum_required(VERSION 3.12)

set (PROJECT examples)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

add_subdirectory(parallelPipeline)
add_subdirectory(scalableParallelPipeline)
add_subdirectory(scalableDynamicParallelPipeline)