cmake_minimum_required(VERSION 3.12)

set (PROJECT CreateDynamicLibrary)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

FIND_PACKAGE(OpenCV REQUIRED)

add_library(UserDL MODULE CreateDL.cpp)

target_link_libraries(UserDL PUBLIC ${OpenCV_LIBS})
target_include_directories(UserDL PUBLIC ${OpenCV_INCLUDE_DIRS})