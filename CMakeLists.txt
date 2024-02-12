cmake_minimum_required(VERSION 3.12)

set (PROJECT Conveyor)

#set(CMAKE_CXX_COMPILER_ID "msvc")

project(${PROJECT} VERSION 1.0 LANGUAGES CXX)

set (CMAKE_CXX_FLAGS "-std=c++20 -Wall ${CMAKE_CXX_FLAGS}")

# for VS
source_group ("Header Files" FILES ${HEADERS})	#1
source_group ("Source Files" FILES ${SOURCES})	#2

include_directories (include/)
 
set (LIBRARIES
		#opencv
		#ИМЯ_БИБЛИОТЕКИ_2
)
 
foreach (LIBRARY ${LIBRARIES})
	add_subdirectory (../${LIBRARY}/build bin/${LIBRARY})
endforeach ()
 
set (HEADERS
		include/conveyor/conveyor.hpp
		include/opencv/opencv.hpp)
set (SOURCES 
		src/source.cpp)

add_executable (${PROJECT} ${HEADERS} ${SOURCES})
 
target_link_libraries (${PROJECT} ${LIBRARIES})