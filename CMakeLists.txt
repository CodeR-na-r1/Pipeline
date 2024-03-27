cmake_minimum_required(VERSION 3.12)

set (PROJECT PipeLine)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

# dependency search
FIND_PACKAGE(OpenCV REQUIRED)

find_package(xtensor REQUIRED)

find_package(CapnProto REQUIRED)

find_package(cppzmq REQUIRED)

#set(MyBoostHeaders "regex" "filesystem")
#find_package(Boost 1.70 REQUIRED COMPONENTS ${MyBoostHeaders})

find_package(Boost 1.70 REQUIRED COMPONENTS ALL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable (Source source.cpp)

add_executable (Source2 sourceGenerator.cpp)

# target_include_directories(Source2 PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Source2 PRIVATE ${OpenCV_LIBS})
target_include_directories(Source2 PRIVATE ${OpenCV_INCLUDE_DIRS})

target_include_directories(Source2 PRIVATE ${xtensor_INCLUDE_DIRS})
target_include_directories(Source2 PRIVATE ${xtl_INCLUDE_DIRS})

target_link_libraries(Source2 PRIVATE CapnProto::capnp)
target_include_directories(Source2 PRIVATE ${CAPNP_INCLUDE_DIRS})

target_link_libraries(Source2 PRIVATE cppzmq ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(Source2 PRIVATE ${cppzmq_INCLUDE_DIR})

target_link_libraries(Source2 PRIVATE ${Boost_LIBRARIES})
target_include_directories(Source2 PRIVATE ${Boost_INCLUDE_DIRS})