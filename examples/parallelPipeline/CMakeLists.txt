cmake_minimum_required(VERSION 3.12)

set (PROJECT ExampleParallelPipeline)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-std=c++20 -Wall -ggdb3 ${CMAKE_CXX_FLAGS}") # -ggdb3

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable (ExampleParallelPipeline Source.cpp)

target_include_directories(ExampleParallelPipeline PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(ExampleParallelPipeline PRIVATE ${OpenCV_LIBS})
target_include_directories(ExampleParallelPipeline PRIVATE ${OpenCV_INCLUDE_DIRS})

target_include_directories(ExampleParallelPipeline PRIVATE ${xtensor_INCLUDE_DIRS})
target_include_directories(ExampleParallelPipeline PRIVATE ${xtl_INCLUDE_DIRS})

target_link_libraries(ExampleParallelPipeline PRIVATE CapnProto::capnp)
target_include_directories(ExampleParallelPipeline PRIVATE ${CAPNP_INCLUDE_DIRS})

target_link_libraries(ExampleParallelPipeline PRIVATE cppzmq ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(ExampleParallelPipeline PRIVATE ${cppzmq_INCLUDE_DIR})

target_link_libraries(ExampleParallelPipeline PRIVATE ${Boost_LIBRARIES})
target_include_directories(ExampleParallelPipeline PRIVATE ${Boost_INCLUDE_DIRS})