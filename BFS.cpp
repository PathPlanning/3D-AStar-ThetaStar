#include "BFS.h"

BFS::BFS(double weight, int BT, int SL) {
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;
    is_any_angle_search = false;
}

void BFS::add_successors_to_opened(const extNode &pos, minqueue &opened,
                                   const std::map<vertex, std::pair<double, vertex>> &,
                                   const EnvironmentOptions &options,
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
            upd.F = upd.H;
            upd.parent = pos.coord;
            opened.push(upd);

        }
    }
}