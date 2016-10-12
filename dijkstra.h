#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "gl_const.h"
#include "isearch.h"

class Dijkstra : public ISearch {
public:
    Dijkstra(int i);

    ~Dijkstra(void);

    virtual void addOpen(Node newNode);

    virtual double computeHFromCellToCell(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h,
                                          const EnvironmentOptions &options);
};

#endif
