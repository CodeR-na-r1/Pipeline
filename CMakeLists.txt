cmake_minimum_required(VERSION 3.12)

set (PROJECT PipeLine)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

# for VS
source_group ("Header Files" FILES ${HEADERS})	#1
source_group ("Source Files" FILES ${SOURCES})	#2

include_directories (include/)
  
set (HEADERS
		${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline/pipeline.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/pipeline/stage.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/parser/parser.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/include/opencv/opencv.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/include/json/json.hpp)
set (SOURCES 
		source.cpp)

add_executable (Source source.cpp ${HEADERS} ${SOURCES})
target_include_directories(Source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Source PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)