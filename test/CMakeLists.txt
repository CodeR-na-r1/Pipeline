cmake_minimum_required(VERSION 3.12)

set (PROJECT Tests)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

find_package (Boost COMPONENTS unit_test_framework REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

include_directories(${Boost_INCLUDE_DIRS})

add_executable (stageTest src/pipeline/StageTest.cpp)   # stage test
target_link_libraries(stageTest PRIVATE ${Boost_LIBRARIES})

add_executable (parserTest src/parser/ParserTest.cpp)   # parser test
target_link_libraries(parserTest PRIVATE ${Boost_LIBRARIES})

add_executable (frameTest src/pipeline/frame/FrameTest.cpp)   # frame (DAO) test
target_link_libraries(frameTest PRIVATE ${Boost_LIBRARIES})

add_executable (connectorTest src/connector/ConnectorTest.cpp)   # connector test
target_link_libraries(connectorTest PRIVATE ${Boost_LIBRARIES})

add_test(NAME StageTest COMMAND stageTest)
add_test(NAME ParserTest COMMAND parserTest)
add_test(NAME FrameTest COMMAND frameTest)
add_test(NAME ConnectorTest COMMAND connectorTest)
