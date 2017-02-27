#include "dijkstra.h"

Dijkstra::Dijkstra(int i) {
    hweight = 0;
    breakingties = CN_SP_BT_GMAX;
}

Dijkstra::~Dijkstra(void) {
}

double Dijkstra::computeHFromCellToCell(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h,
                                        const EnvironmentOptions &options) {
    return 0;
}
