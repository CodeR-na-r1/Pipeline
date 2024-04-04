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

add_executable (stageTest src/pipeline/stageTest.cpp)
target_link_libraries(stageTest PRIVATE ${Boost_LIBRARIES})

add_executable (parserTest src/parser/parserTest.cpp)
target_link_libraries(parserTest PRIVATE ${Boost_LIBRARIES})

add_test(NAME StageTest COMMAND stageTest)
add_test(NAME ParserTest COMMAND parserTest)

# target_link_libraries(Source2 PRIVATE ${OpenCV_LIBS})
# target_include_directories(Source2 PRIVATE ${OpenCV_INCLUDE_DIRS})

# target_include_directories(Source2 PRIVATE ${xtensor_INCLUDE_DIRS})
# target_include_directories(Source2 PRIVATE ${xtl_INCLUDE_DIRS})

# target_link_libraries(Source2 PRIVATE CapnProto::capnp)
# target_include_directories(Source2 PRIVATE ${CAPNP_INCLUDE_DIRS})

# target_link_libraries(Source2 PRIVATE cppzmq ${CMAKE_THREAD_LIBS_INIT})
# target_include_directories(Source2 PRIVATE ${cppzmq_INCLUDE_DIR})