cmake_minimum_required(VERSION 3.12)

set (PROJECT SetupProject)

project(${PROJECT} VERSION 1.0)

# dependency search
FIND_PACKAGE(OpenCV REQUIRED)

find_package(xtensor REQUIRED)

find_package(CapnProto REQUIRED)

find_package(cppzmq REQUIRED)

#set(MyBoostHeaders "regex" "filesystem")
#find_package(Boost 1.70 REQUIRED COMPONENTS ${MyBoostHeaders})

find_package(Boost 1.70 REQUIRED COMPONENTS ALL)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all")
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib CACHE PATH "Single Directory for all")

enable_testing()

add_subdirectory(src)
add_subdirectory(benchmark)
add_subdirectory(test)