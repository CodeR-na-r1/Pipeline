cmake_minimum_required(VERSION 3.12)

set (PROJECT Benchmarks)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_executable (benchmarkLockfreeQueues src/benchmarkLockfreeQueues/benchmarkLockfreeQueues.cpp)
add_executable (benchmarkStageDataType src/benchmarkStageDataType/benchmarkStageDataType.cpp)
add_executable (benchmarkSynchronizeConnectors src/benchmarkSynchronizeConnectors/benchmarkSynchronizeConnectors.cpp)

target_include_directories(benchmarkStageDataType PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmarkStageDataType/include)

target_link_libraries(benchmarkStageDataType PRIVATE ${OpenCV_LIBS})
target_include_directories(benchmarkStageDataType PRIVATE ${OpenCV_INCLUDE_DIRS})

target_include_directories(benchmarkStageDataType PRIVATE ${xtensor_INCLUDE_DIRS})
target_include_directories(benchmarkStageDataType PRIVATE ${xtl_INCLUDE_DIRS})

target_link_libraries(benchmarkStageDataType PRIVATE CapnProto::capnp)
target_include_directories(benchmarkStageDataType PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(benchmarkStageDataType PRIVATE ${CAPNP_INCLUDE_DIRS})

target_link_libraries(benchmarkStageDataType PRIVATE cppzmq ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(benchmarkStageDataType PRIVATE ${cppzmq_INCLUDE_DIR})

target_link_libraries(benchmarkLockfreeQueues PRIVATE ${Boost_LIBRARIES})
target_link_libraries(benchmarkStageDataType PRIVATE ${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})