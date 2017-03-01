#-------------------------------------------------
#
# Project created by QtCreator 2011-02-26T12:08:02
#
#-------------------------------------------------

TARGET = ASearch
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11 -O3

win32 {
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
}

SOURCES += \
    tinyxml2.cpp \
    node.cpp \
    xmllogger.cpp \
    isearch.cpp \
    mission.cpp \
    map.cpp \
    list.cpp \
    config.cpp \
    astar.cpp \
    asearch.cpp \
    environmentoptions.cpp \
    Bresenham.cpp \
    theta.cpp \
    dijkstra.cpp \
    bfs.cpp

HEADERS += \
    tinyxml2.h \
    node.h \
    gl_const.h \
    xmllogger.h \
    isearch.h \
    mission.h \
    map.h \
    ilogger.h \
    list.h \
    config.h \
    astar.h \
    searchresult.h \
    environmentoptions.h \
    Bresenham.h \
    theta.h \
    dijkstra.h \
    bfs.h
