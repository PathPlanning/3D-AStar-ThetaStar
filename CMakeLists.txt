cmake_minimum_required(VERSION 3.5)
project(heuristic_search)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(TinyXML2)

INCLUDE_DIRECTORIES(TinyXML2)
LINK_DIRECTORIES(TinyXML2)

set(SOURCE_FILES main.cpp)
add_executable(heuristic_search ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(heuristic_search tinyxml2)