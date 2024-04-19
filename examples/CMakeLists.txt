cmake_minimum_required(VERSION 3.12)

set (PROJECT examples)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

#add_executable (Source Source.cpp)

# target_include_directories(Source PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#target_link_libraries(Source PRIVATE ${OpenCV_LIBS})
#target_include_directories(Source PRIVATE ${OpenCV_INCLUDE_DIRS})

#target_include_directories(Source PRIVATE ${xtensor_INCLUDE_DIRS})
#target_include_directories(Source PRIVATE ${xtl_INCLUDE_DIRS})

#target_link_libraries(Source PRIVATE CapnProto::capnp)
#target_include_directories(Source PRIVATE ${CAPNP_INCLUDE_DIRS})

#target_link_libraries(Source PRIVATE cppzmq ${CMAKE_THREAD_LIBS_INIT})
#target_include_directories(Source PRIVATE ${cppzmq_INCLUDE_DIR})

#target_link_libraries(Source PRIVATE ${Boost_LIBRARIES})
#target_include_directories(Source PRIVATE ${Boost_INCLUDE_DIRS})