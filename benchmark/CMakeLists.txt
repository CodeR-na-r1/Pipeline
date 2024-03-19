cmake_minimum_required(VERSION 3.12)

set (PROJECT Benchmarks)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

# dependency search
FIND_PACKAGE(OpenCV REQUIRED)

find_package(xtensor REQUIRED)

find_package(CapnProto REQUIRED)

find_package(cppzmq REQUIRED)

find_package(Boost 1.70 REQUIRED COMPONENTS ALL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable (testLockfreeQueues src/testLockfreeQueues.cpp)
add_executable (testStageDataType src/testStageDataType.cpp)

target_link_libraries(testLockfreeQueues PUBLIC ${OpenCV_LIBS})
target_link_libraries(testStageDataType PUBLIC ${OpenCV_LIBS})
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(${xtensor_INCLUDE_DIRS})
include_directories(${xtl_INCLUDE_DIRS})

target_link_libraries(testLockfreeQueues PUBLIC CapnProto::capnp)
target_link_libraries(testStageDataType PUBLIC CapnProto::capnp)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CAPNP_INCLUDE_DIRS})

target_link_libraries(testLockfreeQueues PUBLIC cppzmq ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(testStageDataType PUBLIC cppzmq ${CMAKE_THREAD_LIBS_INIT})
include_directories(${cppzmq_INCLUDE_DIR})

target_link_libraries(testLockfreeQueues PUBLIC ${Boost_LIBRARIES})
target_link_libraries(testStageDataType PUBLIC ${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})