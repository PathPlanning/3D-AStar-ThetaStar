#include "astar.h"
#include <stdlib.h>
#include <math.h>
#include "isearch.h"
#include <iostream>

Astar::Astar() : ISearch() {}

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
    std::list<Node>::iterator iter, pos;

    bool posFound = false;

    pos = open[newNode.i].List.end();

    if (open[newNode.i].List.empty()) {
        open[newNode.i].List.push_back(newNode);
        openSize++;
        return;
    }

    for (iter = open[newNode.i].List.begin(); iter != open[newNode.i].List.end(); ++iter) {
        if ((iter->F >= newNode.F) && (!posFound)) {
            if (iter->F == newNode.F) {
                switch (breakingties) {
                    case CN_SP_BT_GMAX: {
                        if (newNode.g >= iter->g) {
                            pos = iter;
                            posFound = true;
                        }

                    }
                    case CN_SP_BT_GMIN: {
                        if (newNode.g <= iter->g) {
                            pos = iter;
                            posFound = true;
                        }
                    }
                }
            } else {
                pos = iter;
                posFound = true;
            }
        }

        if (((iter->i) == newNode.i) && (iter->j) == newNode.j && iter->z == newNode.z) {
            if (newNode.F >= iter->F) {
                return;
            } else {
                if (pos == iter) {
                    iter->F = newNode.F;
                    iter->g = newNode.g;
                    iter->parent = newNode.parent;
                    return;
                }
                open[newNode.i].List.erase(iter);
                openSize--;
                break;
            }
        }
    }
    openSize++;
    open[newNode.i].List.insert(pos, newNode);
}
