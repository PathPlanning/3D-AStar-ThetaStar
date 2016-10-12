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

void Dijkstra::addOpen(Node newNode) {
    bool posFound = false;

    auto pos = open[newNode.i].List.end();

    if (open[newNode.i].List.empty()) {
        open[newNode.i].List.push_back(newNode);
        ++openSize;
        return;
    }

    for (auto iter = open[newNode.i].List.begin(); iter != open[newNode.i].List.end(); ++iter) {
        if ((iter->g >= newNode.g) && (!posFound)) {
            pos = iter;
            posFound = true;
        }

        if (((iter->i) == newNode.i) && (iter->j) == newNode.j && (iter->z) == newNode.z) {
            if (newNode.g >= iter->g)
                return;
            else {
                if (pos == iter) {
                    iter->F = newNode.F;
                    iter->g = newNode.g;
                    iter->parent = newNode.parent;
                    return;
                }
                open[newNode.i].List.erase(iter);
                --openSize;
                break;
            }
        }
    }

    open[newNode.i].List.insert(pos, newNode);
    ++openSize;
}
