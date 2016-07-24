#include "astar.h"

#include <algorithm>
#include <math.h>

Astar::Astar() : ISearch() { }

Astar::Astar(double w, int BT, int SL) {
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
}

void Astar::add_successors_to_opened(const extNode &pos, minqueue &opened, const EnvironmentOptions &options,
                                     const Map &map) {
    int i = pos.coord.i;
    int j = pos.coord.j;
    bool is_diag_move;
    extNode upd;
    for (int add_i = -1; add_i != 2; ++add_i) {
        for (int add_j = -1; add_j != 2; ++add_j) {
            // Skip the same point
            if (add_i == 0 && add_j == 0) {
                continue;
            }

            is_diag_move = add_i != 0 && add_j != 0;
            // Skip diagonal move, if not allowed
            if (is_diag_move && !options.allowdiagonal) {
                continue;
            }

            if (!map.CellOnGrid(i + add_i, j + add_j) || map.CellIsObstacle(i + add_i, j + add_j)) {
                continue;
            }

            // Squeeze check
            if (is_diag_move && !options.allowsqueeze &&
                map.CellIsObstacle(i + add_i, j) && map.CellIsObstacle(i, j + add_j)) {
                continue;
            }


            upd.coord = {i + add_i, j + add_j};
            upd.g = pos.g + (is_diag_move ? options.diagonalcost : options.linecost);
            upd.H = heuristic(i + add_i, j + add_j, map.goal_i, map.goal_j, options);
            upd.parent = pos.coord;
            upd.calc_F();
            opened.push(upd);

        }
    }
}

Dijkstra::Dijkstra(double weight, int BT, int SL) {
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;
}

inline double Dijkstra::heuristic(int, int, int, int, const EnvironmentOptions &) const {
    return 0.0;
}
