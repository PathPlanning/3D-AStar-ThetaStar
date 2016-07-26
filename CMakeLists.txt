cmake_minimum_required(VERSION 3.2)
project(HeuristicSearch)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -o2")

set(SOURCE_FILES
        asearch.cpp
        config.cpp
        config.h
        environmentoptions.cpp
        environmentoptions.h
        gl_const.h
        ilogger.h
        isearch.cpp
        isearch.h
        list.cpp
        list.h
        map.cpp
        map.h
        mission.cpp
        mission.h
        node.h
        searchresult.h
        tinystr.cpp
        tinystr.h
        tinyxml.cpp
        tinyxml.h
        tinyxmlerror.cpp
        tinyxmlparser.cpp
        xmllogger.cpp
        xmllogger.h
        minqueue.cpp
        minqueue.h
        JPS.cpp
        JPS.h
        astar.cpp
        astar.h
        Theta_star.cpp
        Theta_star.h
        BFS.cpp
        BFS.h)

add_executable(HeuristicSearch ${SOURCE_FILES})
