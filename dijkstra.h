#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include "gl_const.h"
#include "isearch.h"

class Dijkstra : public ISearch
{
    public:
        Dijkstra(int i);
        ~Dijkstra(void);

        void addOpen(Node newNode);
        double computeHFromCellToCell(int start_i, int start_j, int fin_i, int fin_j, const EnvironmentOptions &options);
};
#endif
