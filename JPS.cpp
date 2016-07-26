#include "JPS.h"

JPS::JPS(double w, int BT, int SL) {
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
    is_any_angle_search = false;
}

std::pair<int, int> JPS::corner_directions(int direction) const {
    switch (direction) {
        case CN_MD_UP_RIGHT:
            return {CN_MD_UP, CN_MD_RIGHT};
        case CN_MD_DOWN_RIGHT:
            return {CN_MD_RIGHT, CN_MD_DOWN};
        case CN_MD_DOWN_LEFT:
            return {CN_MD_DOWN, CN_MD_LEFT};
        case CN_MD_UP_LEFT:
            return {CN_MD_LEFT, CN_MD_UP};
    }
};

std::pair<vertex, bool> JPS::jump(vertex position, int direction, const vertex &goal,
                                  const EnvironmentOptions &options, const Map &map) const {
    while (true) {
        position = make_one_step(position, direction);
        if (!map.CellOnGrid(position.i, position.j) || map.CellIsObstacle(position.i, position.j)) {
            return {position, false};
        }
        if (position == goal ||
            has_forced_neigbour(position, direction, options.allowsqueeze, map)) {
            return {position, true};
        }

        if (is_diagonal_move(direction)) {
            auto corner_dirs = corner_directions(direction);
            if (jump(position, corner_dirs.first, goal, options, map).second ||
                jump(position, corner_dirs.second, goal, options, map).second) {
                return {position, true};
            }
        }
    }
}

/*
 * Return true if neighbour must be pruned. Not guarantee that neighbour is traversable
 */
bool JPS::is_pruned_neigbour(const vertex &x, const vertex &neighbour, int direction, const Map &map) const {
    if (x.i == map.start_i && x.j == map.start_j) {
        return false;
    }
    int delta_i = neighbour.i - x.i;
    int delta_j = neighbour.j - x.j;
    switch (direction) {
        case CN_MD_RIGHT:
            if (delta_i != 0) {
                return !(delta_j == 1 && map.CellIsObstacle(neighbour.i, x.j));
            }
            return delta_j <= 0;
        case CN_MD_LEFT:
            if (delta_i != 0) {
                return !(delta_j == -1 && map.CellIsObstacle(neighbour.i, x.j));
            }
            return delta_j >= 0;
        case CN_MD_UP:
            if (delta_j != 0) {
                return !(delta_i == -1 && map.CellIsObstacle(x.i, neighbour.j));
            }
            return delta_i >= 0;
        case CN_MD_DOWN:
            if (delta_j != 0) {
                return !(delta_i == 1 && map.CellIsObstacle(x.i, neighbour.j));
            }
            return delta_i <= 0;

            //Diagonal directions
        case CN_MD_UP_RIGHT:
            if (delta_i == 1) {
                if (delta_j == -1) {
                    return true;
                }
                return !(delta_j == 1 && map.CellIsObstacle(neighbour.i, neighbour.j - 1));
            }
            if (delta_j == -1) {
                return !(delta_i == -1 && map.CellIsObstacle(neighbour.i + 1, neighbour.j));
            }
            return false;
        case CN_MD_DOWN_RIGHT:
            if (delta_i == -1) {
                if (delta_j == -1) {
                    return true;
                }
                return !(delta_j == 1 && map.CellIsObstacle(neighbour.i, neighbour.j - 1));
            }
            if (delta_j == -1) {
                return !(delta_i == 1 && map.CellIsObstacle(neighbour.i - 1, neighbour.j));
            }
            return false;
        case CN_MD_DOWN_LEFT:
            if (delta_i == -1) {
                if (delta_j == 1) {
                    return true;
                }
                return !(delta_j == -1 && map.CellIsObstacle(neighbour.i, neighbour.j + 1));
            }
            if (delta_j == 1) {
                return !(delta_i == 1 && map.CellIsObstacle(neighbour.i - 1, neighbour.j));
            }
            return false;
        case CN_MD_UP_LEFT:
            if (delta_i == 1) {
                if (delta_j == 1) {
                    return true;
                }
                return !(delta_j == -1 && map.CellIsObstacle(neighbour.i, neighbour.j + 1));
            }
            if (delta_j == 1) {
                return !(delta_i == -1 && map.CellIsObstacle(neighbour.i + 1, neighbour.j));
            }
            return false;
        case CN_MD_STATIC:
            return false;
    }
}

bool JPS::has_forced_neigbour(const vertex &x, int direction, bool squeeze, const Map &map) const {
    switch (direction) {
        case CN_MD_RIGHT:
            return !(!squeeze && map.CellOnGrid(x.i, x.j + 1) && map.CellIsObstacle(x.i, x.j + 1)) &&
                   ((map.CellOnGrid(x.i - 1, x.j + 1) && map.CellIsTraversable(x.i - 1, x.j + 1) &&
                     map.CellIsObstacle(x.i - 1, x.j))
                    ||
                    (map.CellOnGrid(x.i + 1, x.j + 1) && map.CellIsTraversable(x.i + 1, x.j + 1) &&
                     map.CellIsObstacle(x.i + 1, x.j)));
        case CN_MD_LEFT:
            return !(!squeeze && map.CellOnGrid(x.i, x.j - 1) && map.CellIsObstacle(x.i, x.j - 1)) &&
                   ((map.CellOnGrid(x.i - 1, x.j - 1) && map.CellIsTraversable(x.i - 1, x.j - 1) &&
                     map.CellIsObstacle(x.i - 1, x.j))
                    ||
                    (map.CellOnGrid(x.i + 1, x.j - 1) && map.CellIsTraversable(x.i + 1, x.j - 1) &&
                     map.CellIsObstacle(x.i + 1, x.j)));
        case CN_MD_UP:
            return !(!squeeze && map.CellOnGrid(x.i - 1, x.j) && map.CellIsObstacle(x.i - 1, x.j)) &&
                   ((map.CellOnGrid(x.i - 1, x.j + 1) && map.CellIsTraversable(x.i - 1, x.j + 1) &&
                     map.CellIsObstacle(x.i, x.j + 1))
                    ||
                    (map.CellOnGrid(x.i - 1, x.j - 1) && map.CellIsTraversable(x.i - 1, x.j - 1) &&
                     map.CellIsObstacle(x.i, x.j - 1)));
        case CN_MD_DOWN:
            return !(!squeeze && map.CellOnGrid(x.i + 1, x.j) && map.CellIsObstacle(x.i + 1, x.j)) &&
                   ((map.CellOnGrid(x.i + 1, x.j + 1) && map.CellIsTraversable(x.i + 1, x.j + 1) &&
                     map.CellIsObstacle(x.i, x.j + 1))
                    ||
                    (map.CellOnGrid(x.i + 1, x.j - 1) && map.CellIsTraversable(x.i + 1, x.j - 1) &&
                     map.CellIsObstacle(x.i, x.j - 1)));
        case CN_MD_UP_RIGHT:
            return (map.CellOnGrid(x.i + 1, x.j + 1) && map.CellIsTraversable(x.i + 1, x.j + 1) &&
                    map.CellIsObstacle(x.i + 1, x.j) && !(!squeeze && map.CellIsObstacle(x.i, x.j + 1))) ||
                   (map.CellOnGrid(x.i - 1, x.j - 1) && map.CellIsTraversable(x.i - 1, x.j - 1) &&
                    map.CellIsObstacle(x.i, x.j - 1) && !(!squeeze && map.CellIsObstacle(x.i - 1, x.j)));
        case CN_MD_DOWN_RIGHT:
            return (map.CellOnGrid(x.i + 1, x.j - 1) && map.CellIsTraversable(x.i + 1, x.j - 1) &&
                    map.CellIsObstacle(x.i, x.j - 1) && !(!squeeze && map.CellIsObstacle(x.i + 1, x.j))) ||
                   (map.CellOnGrid(x.i - 1, x.j + 1) && map.CellIsTraversable(x.i - 1, x.j + 1) &&
                    map.CellIsObstacle(x.i - 1, x.j) && !(!squeeze && map.CellIsObstacle(x.i, x.j + 1)));
        case CN_MD_DOWN_LEFT:
            return (map.CellOnGrid(x.i + 1, x.j + 1) && map.CellIsTraversable(x.i + 1, x.j + 1) &&
                    map.CellIsObstacle(x.i, x.j + 1) && !(!squeeze && map.CellIsObstacle(x.i + 1, x.j))) ||
                   (map.CellOnGrid(x.i - 1, x.j - 1) && map.CellIsTraversable(x.i - 1, x.j - 1) &&
                    map.CellIsObstacle(x.i - 1, x.j) && !(!squeeze && map.CellIsObstacle(x.i, x.j - 1)));
        case CN_MD_UP_LEFT:
            return (map.CellOnGrid(x.i + 1, x.j - 1) && map.CellIsTraversable(x.i + 1, x.j - 1) &&
                    map.CellIsObstacle(x.i + 1, x.j) && !(!squeeze && map.CellIsObstacle(x.i, x.j + 1))) ||
                   (map.CellOnGrid(x.i - 1, x.j + 1) && map.CellIsTraversable(x.i - 1, x.j + 1) &&
                    map.CellIsObstacle(x.i, x.j + 1) && !(!squeeze && map.CellIsObstacle(x.i - 1, x.j)));
        case CN_MD_STATIC:
            return false;
    }
}

void JPS::add_successors_to_opened(const extNode &pos,
                                   minqueue &opened,
                                   const std::map<vertex, std::pair<double, vertex>> &,
                                   const EnvironmentOptions &options,
                                   const Map &map) {
    int i = pos.coord.i;
    int j = pos.coord.j;
    //std::cout << '(' << i << ", " << j << "). Visiting neighbours:\n";
    vertex goal = {map.goal_i, map.goal_j};
    extNode upd;
    int neigbour_dir, parent_dir;
    bool is_diag_move;
    std::pair<vertex, bool> jump_point;

    parent_dir = direction(pos.parent, pos.coord);
    for (int add_i = -1; add_i != 2; ++add_i) {
        for (int add_j = -1; add_j != 2; ++add_j) {
            // Skip the same point
            if (add_i == 0 && add_j == 0) {
                continue;
            }

            if (!map.CellOnGrid(i + add_i, j + add_j) || map.CellIsObstacle(i + add_i, j + add_j)) {
                continue;
            }

            is_diag_move = (add_i != 0 && add_j != 0);
            // Squeeze check
            if (is_diag_move && !options.allowsqueeze &&
                map.CellIsObstacle(i + add_i, j) && map.CellIsObstacle(i, j + add_j)) {
                continue;
            }

            if (!is_pruned_neigbour(pos.coord, {i + add_i, j + add_j}, parent_dir, map)) {
                neigbour_dir = direction(add_i, add_j);
                //std::cout << parent_dir << ' ' << neigbour_dir << "\t(" << i + add_i << ", " << j + add_j << ")\t";
                jump_point = jump(pos.coord, neigbour_dir, goal, options, map);
                //std::cout << jump_point.second << " (" << jump_point.first.i << ", " << jump_point.first.j << ")\n";
                if (jump_point.second) {
                    upd.coord = jump_point.first;
                    upd.g = pos.g + (is_diag_move ? options.diagonalcost * distance(pos.coord, jump_point.first) :
                                     options.linecost * distance(pos.coord, jump_point.first));
                    upd.H = heuristic(jump_point.first.i, jump_point.first.j, map.goal_i, map.goal_j, options);
                    upd.parent = pos.coord;
                    upd.calc_F();
                    opened.push(upd);
                }
            }
        }
    }
}

orthogonalJPS::orthogonalJPS(double weight, int BT, int SL) {
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;
}

bool orthogonalJPS::is_horizontal_direction(int direction) const {
    switch (direction) {
        case CN_MD_RIGHT:
        case CN_MD_LEFT:
            return true;
        default:
            return false;
    }
}

bool orthogonalJPS::is_vertical_direction(int direction) const {
    switch (direction) {
        case CN_MD_UP:
        case CN_MD_DOWN:
            return true;
        default:
            return false;
    }
}

std::pair<vertex, bool> orthogonalJPS::jump(vertex position, int direction, const vertex &goal,
                                            const EnvironmentOptions &options, const Map &map) const {
    vertex prev;
    while (true) {
        prev = position;
        position = make_one_step(position, direction);
        if (!map.CellOnGrid(position.i, position.j) || map.CellIsObstacle(position.i, position.j)) {
            return {position, false};
        }
        if (position == goal) {
            return {position, true};
        }
        if (is_horizontal_direction(direction) &&
            (map.CellOnGrid(prev.i - 1, prev.j) && map.CellIsObstacle(prev.i - 1, prev.j) ||
             map.CellOnGrid(prev.i + 1, prev.j) && map.CellIsObstacle(prev.i + 1, prev.j))) {
            return {position, true};
        }
        if (is_vertical_direction(direction) && (jump(position, CN_MD_LEFT, goal, options, map).second ||
                                                 jump(position, CN_MD_RIGHT, goal, options, map).second)) {
            return {position, true};
        }
    }
}

/*
 * Return true if neighbour must be pruned. Not guarantee that neighbour is traversable
 */
bool orthogonalJPS::is_pruned_neigbour(const vertex &x, const vertex &neighbour, int direction, const Map &map) const {
    if (x.i == map.start_i && x.j == map.start_j) {
        return false;
    }
    int delta_i = neighbour.i - x.i;
    int delta_j = neighbour.j - x.j;
    switch (direction) {
        case CN_MD_RIGHT:
            if (delta_i != 0) {
                return !(delta_j == 0 || delta_j == 1 && map.CellIsObstacle(neighbour.i, x.j));
            }
            return delta_j <= 0;
        case CN_MD_LEFT:
            if (delta_i != 0) {
                return !(delta_j == 0 || delta_j == -1 && map.CellIsObstacle(neighbour.i, x.j));
            }
            return delta_j >= 0;
        case CN_MD_UP:
            if (delta_j != 0) {
                return !(delta_i == 0 || delta_i == -1 && map.CellIsObstacle(x.i, neighbour.j));
            }
            return delta_i >= 0;
        case CN_MD_DOWN:
            if (delta_j != 0) {
                return !(delta_i == 0 || delta_i == 1 && map.CellIsObstacle(x.i, neighbour.j));
            }
            return delta_i <= 0;

        case CN_MD_STATIC:
            return false;
    }
}

void orthogonalJPS::add_successors_to_opened(const extNode &pos,
                                             minqueue &opened,
                                             const std::map<vertex, std::pair<double, vertex>> &,
                                             const EnvironmentOptions &options,
                                             const Map &map) {
    int i = pos.coord.i;
    int j = pos.coord.j;
    //std::cout << '(' << i << ", " << j << "). Visiting neighbours:\n";
    vertex goal = {map.goal_i, map.goal_j};
    extNode upd;
    int neigbour_dir, parent_dir;
    std::pair<vertex, bool> jump_point;

    parent_dir = direction(pos.parent, pos.coord);
    for (int add_i = -1; add_i != 2; ++add_i) {
        for (int add_j = -1; add_j != 2; ++add_j) {
            // Skip the same point
            if (add_i == 0 && add_j == 0) {
                continue;
            }

            if (!map.CellOnGrid(i + add_i, j + add_j) || map.CellIsObstacle(i + add_i, j + add_j)) {
                continue;
            }

            if (add_i != 0 && add_j != 0) {
                continue;
            }
            if (!is_pruned_neigbour(pos.coord, {i + add_i, j + add_j}, parent_dir, map)) {
                neigbour_dir = direction(add_i, add_j);
                //std::cout << parent_dir << ' ' << neigbour_dir << "\t(" << i + add_i << ", " << j + add_j << ")\t";
                jump_point = jump(pos.coord, neigbour_dir, goal, options, map);
                //std::cout << jump_point.second << " (" << jump_point.first.i << ", " << jump_point.first.j << ")\n";
                if (jump_point.second) {
                    upd.coord = jump_point.first;
                    upd.g = pos.g + options.linecost * distance(pos.coord, jump_point.first);
                    upd.H = heuristic(jump_point.first.i, jump_point.first.j, map.goal_i, map.goal_j, options);
                    upd.parent = pos.coord;
                    upd.calc_F();
                    opened.push(upd);
                }
            }
        }
    }
}