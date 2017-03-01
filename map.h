#ifndef MAP_H
#define	MAP_H
#include "tinyxml2.h"
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
        bool CellIsTraversable (int i, int j, int h) const;
        bool CellOnGrid (int i, int j) const;
        bool CellOnGrid (int i, int j, int height) const;
        bool CellIsObstacle(int i, int j) const;
        bool CellIsObstacle(int i, int j, int h) const;
        int  getValue(int i, int j) const;

    public:
        int**   Grid;
        int     height, width, altitude;
        int     min_altitude_limit, max_altitude_limit; // The lowest and highest possible altitude for the path
        int     start_i, start_j, start_h;
        int     goal_i, goal_j, goal_h;
};

#endif

