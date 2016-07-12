#ifndef MAP_H
#define	MAP_H
#include "tinystr.h"
#include "tinyxml.h"
#include <iostream>
#include "gl_const.h"
#include <sstream>
#include <string>
#include <algorithm>
class Map
{
    public:
        Map();
        Map(const Map& orig);
        ~Map();

        bool getMap(const char *FileName);
        bool CellIsTraversable (int i, int j) const;
        bool CellOnGrid (int i, int j) const;
        bool CellIsObstacle(int i, int j) const;
        int  getValue(int i, int j) const;

    public:
        int**   Grid;
        int     height, width;
        int     start_i, start_j;
        int     goal_i, goal_j;
};

#endif

