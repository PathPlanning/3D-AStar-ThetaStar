#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "astar.h"
#include "gl_const.h"

class Dijkstra : public Astar {
public:
    Dijkstra(int i);

    ~Dijkstra(void);

    virtual double computeHFromCellToCell(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h,
                                          const EnvironmentOptions &options);
};

#endif
