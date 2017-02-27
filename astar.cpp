#include "astar.h"
#include <stdlib.h>
#include <math.h>
#include "isearch.h"
#include <iostream>

Astar::Astar() : ISearch() { }

Astar::Astar(double w, int BT, int SL, int i) {
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
}

Astar::~Astar() {
}

double Astar::computeHFromCellToCell(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h,
                                     const EnvironmentOptions &options) {
    if (options.metrictype == CN_SP_MT_MANH) {
        return options.linecost * (abs(fin_i - start_i) + abs(fin_j - start_j) + abs(fin_h - start_h));
    }
    if (options.metrictype == CN_SP_MT_CHEB) {
        return options.linecost * std::max(std::max(abs(fin_i - start_i), abs(fin_j - start_j)), abs(fin_h - start_h));
    }
    if (options.metrictype == CN_SP_MT_EUCL) {
        return options.linecost * sqrt((fin_i - start_i) * (fin_i - start_i) + (fin_j - start_j) * (fin_j - start_j) +
                                       (fin_h - start_h) * (fin_h - start_h));
    }
    if (options.metrictype == CN_SP_MT_DIAG) {
        int d_i = abs(fin_i - start_i);
        int d_j = abs(fin_j - start_j);
        int d_h = abs(fin_h - start_h);
        int diag = std::min(std::min(d_i, d_j), d_h);
        d_i -= diag;
        d_j -= diag;
        d_h -= diag;
        if (d_i == 0) {
            return options.linecost * sqrt(3) * diag + options.diagonalcost * std::min(d_j, d_h) +
                   options.linecost * abs(d_j - d_h);
        }
        if (d_j == 0) {
            return options.linecost * sqrt(3) * diag + options.diagonalcost * std::min(d_i, d_h) +
                   options.linecost * abs(d_i - d_h);
        }
        if (d_h == 0) {
            return options.linecost * sqrt(3) * diag + options.diagonalcost * std::min(d_i, d_j) +
                   options.linecost * abs(d_i - d_j);
        }
    }
    return 0;
}

void Astar::addOpen(Node newNode) {
    bool inserted = false;
    size_t idx = newNode.i;
    if (open[idx].find(newNode) != open[idx].end()) {
        if (newNode.F < open[idx].find(newNode)->F) {
            open[idx].erase(newNode);
            open[idx].insert(newNode);
            inserted = true;
        }
    } else {
        open[idx].insert(newNode);
        inserted = true;
        ++openSize;
    }

    if (inserted && newNode.F <= openMinimums[newNode.i].F) {
        if (newNode.F == openMinimums[idx].F) {
            switch (breakingties) {
                default:
                case CN_SP_BT_GMAX: {
                    if (newNode.g >= openMinimums[idx].g) {
                        openMinimums[idx] = newNode;
                    }
                    break;
                }
                case CN_SP_BT_GMIN: {
                    if (newNode.g <= openMinimums[idx].g) {
                        openMinimums[idx] = newNode;
                    }
                    break;
                }
            }
        } else {
            openMinimums[idx] = newNode;
        }
    }
}
