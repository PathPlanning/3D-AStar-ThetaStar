#-------------------------------------------------
#
# Project created by QtCreator 2011-02-26T12:08:02
#
#-------------------------------------------------

TARGET = HeuristicSearch
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11 -o2

win32 {
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
}

SOURCES += \
    tinyxmlparser.cpp \
    tinyxmlerror.cpp \
    tinyxml.cpp \
    tinystr.cpp \
    xmllogger.cpp \
    isearch.cpp \
    mission.cpp \
    map.cpp \
    list.cpp \
    config.cpp \
    JPS.cpp \
    asearch.cpp \
    environmentoptions.cpp \
    prior_queue.cpp \
    astar.cpp \
    BFS.cpp \
    Theta_star.cpp

HEADERS += \
    tinyxml.h \
    tinystr.h \
    node.h \
    gl_const.h \
    xmllogger.h \
    isearch.h \
    mission.h \
    map.h \
    ilogger.h \
    list.h \
    config.h \
    JPS.h \
    searchresult.h \
    environmentoptions.h \
    prior_queue.h \
    astar.h \
    BFS.h \
    Theta_star.h
