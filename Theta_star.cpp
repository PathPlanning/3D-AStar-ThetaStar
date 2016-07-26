#include "Theta_star.h"

#include <math.h>

Theta_star::Theta_star(double weight, int BT, int SL) {
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;
    is_any_angle_search = true;
}

void Theta_star::add_successors_to_opened(const extNode &pos, minqueue &opened,
                                          const std::map<vertex, std::pair<double, vertex>> &closed,
                                          const EnvironmentOptions &options, const Map &map) {
    int i = pos.coord.i;
    int j = pos.coord.j;
    bool is_diag_move;
    extNode upd;

    double parent_path_cost = closed.at(pos.parent).first;
    for (int add_i = -1; add_i != 2; ++add_i) {
        for (int add_j = -1; add_j != 2; ++add_j) {
            // Skip the same point
            if (add_i == 0 && add_j == 0) {
                continue;
            }

            is_diag_move = add_i != 0 && add_j != 0;

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
            if (line_of_sight(pos.parent, upd.coord, map) && parent_path_cost + euclid_distance(pos.parent, upd.coord, options.linecost) < upd.g) {
                upd.g = parent_path_cost + euclid_distance(pos.parent, upd.coord, options.linecost);
                upd.parent = pos.parent;
            } else {
                upd.parent = pos.coord;
            }
            upd.calc_F();
            opened.push(upd);
        }
    }
}