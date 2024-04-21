cmake_minimum_required(VERSION 3.12)

set (PROJECT dataStream)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)

if (NOT IS_SUBPROJECT)

	find_package(OpenCV REQUIRED)

	find_package(CapnProto REQUIRED)

	find_package(cppzmq REQUIRED)

endif()

add_executable (dataStream DataStream.cpp)

target_link_libraries(dataStream PRIVATE ${OpenCV_LIBS})
target_include_directories(dataStream PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(dataStream PRIVATE CapnProto::capnp)
target_include_directories(dataStream PRIVATE ${CAPNP_INCLUDE_DIRS})

target_link_libraries(dataStream PRIVATE cppzmq ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(dataStream PRIVATE ${cppzmq_INCLUDE_DIR})
