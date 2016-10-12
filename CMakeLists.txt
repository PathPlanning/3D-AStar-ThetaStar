cmake_minimum_required(VERSION 3.0)
project(Astar_heights)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O4")

set(SOURCE_FILES
        tinyxmlparser.cpp
        tinyxmlerror.cpp
        tinyxml.cpp
        tinystr.cpp
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
        tinyxml.h
        tinystr.h
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
        bfs.h)
add_executable(Astar_heights ${SOURCE_FILES})