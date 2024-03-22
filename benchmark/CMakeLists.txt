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

add_executable (testLockfreeQueues src/testLockfreeQueues/testLockfreeQueues.cpp)
add_executable (testStageDataType src/testStageDataType/testStageDataType.cpp)

target_include_directories(testStageDataType PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/testStageDataType/include)

target_link_libraries(testStageDataType PRIVATE ${OpenCV_LIBS})
target_include_directories(testStageDataType PRIVATE ${OpenCV_INCLUDE_DIRS})

target_include_directories(testStageDataType PRIVATE ${xtensor_INCLUDE_DIRS})
target_include_directories(testStageDataType PRIVATE ${xtl_INCLUDE_DIRS})

target_link_libraries(testStageDataType PRIVATE CapnProto::capnp)
target_include_directories(testStageDataType PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(testStageDataType PRIVATE ${CAPNP_INCLUDE_DIRS})

target_link_libraries(testStageDataType PRIVATE cppzmq ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(testStageDataType PRIVATE ${cppzmq_INCLUDE_DIR})

target_link_libraries(testLockfreeQueues PRIVATE ${Boost_LIBRARIES})
target_link_libraries(testStageDataType PRIVATE ${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})