cmake_minimum_required(VERSION 2.8)
project(3D-AStar-ThetaStar)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

set(SOURCE_FILES
        node.cpp
        xmllogger.cpp
        isearch.cpp
        mission.cpp
        map.cpp
        list.cpp
        config.cpp
        astar.cpp
        asearch.cpp
        environmentoptions.cpp
        node.h
        gl_const.h
        xmllogger.h
        isearch.h
        mission.h
        map.h
        ilogger.h
        list.h
        config.h
        astar.h
        searchresult.h
        environmentoptions.h
        Bresenham.cpp
        Bresenham.h
        theta.cpp
        theta.h
        dijkstra.cpp
        dijkstra.h
        bfs.cpp
        bfs.h
        Queues.cpp
        Queues.h
        tinyxml2.h
        tinyxml2.cpp)
add_executable(Astar_heights ${SOURCE_FILES})