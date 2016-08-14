#include "gl_const.h"
#include "prior_queue.h"
#include "isearch.h"

#include <vector>
#include <chrono>
#include <utility>
#include <queue>
#include <list>
#include <unordered_map>


ISearch::ISearch() {
}

SearchResult ISearch::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    sresult.pathfound = false;
    sresult.pathlength = 0;

    auto start_time = std::chrono::high_resolution_clock::now();
    decltype(start_time) finish_time;
    std::unordered_map<vertex, Node> closed;
    coord_list_queue opened(breakingties, map.height);

    Node current;
    current.i = map.start_i;
    current.j = map.start_j;
    current.g = 0;
    current.H = heuristic(map.start_i, map.start_j, map.goal_i, map.goal_j, options);
    current.F = current.g + current.H;
    current.parent = nullptr;
    opened.push(current);

    Node* current_ptr;
    std::list<Node> successors;
    while (!opened.empty()) {
        current = opened.top();
        opened.pop();

        closed[{current.i, current.j}] = current;
        current_ptr = &(closed.at({current.i, current.j}));
        if (current.i == map.start_i && current.j == map.start_j) {
            current_ptr->parent = current_ptr;
        }
        if (current_ptr->i == map.goal_i && current_ptr->j == map.goal_j) {
            sresult.pathfound = true;
            sresult.pathlength = current_ptr->g;
            break;
        }
        successors = find_successors(current_ptr, map, options);
        for (auto node : successors) {
            improve_parent(&node, map, options);
            if (closed.find({node.i, node.j}) == closed.end()) {
                opened.push(node);
            }
        }
    }

    if (sresult.pathfound) {
        recovery_primary_path(current_ptr, map.start_i, map.start_j);
        recovery_secondary_path(current_ptr, map.start_i, map.start_j, options);
    } else {
        sresult.pathfound = false;
        sresult.pathlength = 0;
    }

    finish_time = std::chrono::high_resolution_clock::now();
    sresult.time = std::chrono::duration_cast<std::chrono::microseconds>(finish_time - start_time).count();
    sresult.time /= 1000000;
    sresult.numberofsteps = closed.size();
    sresult.nodescreated = closed.size() + opened.size();

    auto foramated_viewed = new std::map<Node, bool>;
    int i, j;
    while (!opened.empty()) {
        foramated_viewed->operator[](opened.top()) = false;
        opened.pop();
    }
    for (auto it = closed.begin(); it != closed.end(); ++it) {
        foramated_viewed->operator[](it->second) = true;
    }

    sresult.viewed = foramated_viewed;

    return sresult;
}

double ISearch::heuristic(int i, int j, int goal_i, int goal_j, const EnvironmentOptions &options) const {
    double result;
    int delta_i, delta_j;
    switch (options.metrictype) {
        case CN_SP_MT_CHEB:
            result = options.linecost * std::max(std::abs(i - goal_i), std::abs(j - goal_j));
            break;
        case CN_SP_MT_MANH:
            result = options.linecost * (std::abs(i - goal_i) + std::abs(j - goal_j));
            break;
        case CN_SP_MT_DIAG:
            delta_i = std::abs(i - goal_i);
            delta_j = std::abs(j - goal_j);
            if (delta_i < delta_j) {
                result = options.diagonalcost * delta_i + options.linecost * (delta_j - delta_i);
            } else {
                result = options.diagonalcost * delta_j + options.linecost * (delta_i - delta_j);
            }
            break;
        default:
        case CN_SP_MT_EUCL:
            result = options.linecost * std::sqrt(std::pow(i - goal_i, 2) + std::pow(j - goal_j, 2));
    }
    return hweight * result;
}

std::list<Node> ISearch::find_successors(Node *current, const Map &map,
                                         const EnvironmentOptions &options) const {
    std::list<Node> result;
    int i = current->i;
    int j = current->j;
    bool is_diag_move;
    Node successor;
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
            successor.i = i + add_i;
            successor.j = j + add_j;
            successor.g = current->g + (is_diag_move ? options.diagonalcost : options.linecost);
            successor.H = heuristic(i + add_i, j + add_j, map.goal_i, map.goal_j, options);
            successor.F = successor.H + successor.g;
            successor.parent = current;
            result.push_back(successor);
        }
    }
    return result;
}

void ISearch::improve_parent(Node *current, const Map &map, const EnvironmentOptions &options) const {
    return;
}

//Only straight and diagonal directions
int ISearch::direction(const vertex &from, const vertex &to) const {
    int delta_i = to.i - from.i;
    int delta_j = to.j - from.j;
    return direction(delta_i, delta_j);
}

int ISearch::direction(int delta_i, int delta_j) const {
    if (delta_i > 0) {
        if (delta_j > 0) {
            return CN_MD_DOWN_RIGHT;
        }
        if (delta_j == 0) {
            return CN_MD_DOWN;
        }
        return CN_MD_DOWN_LEFT;
    }
    if (delta_i == 0) {
        if (delta_j > 0) {
            return CN_MD_RIGHT;
        }
        if (delta_j == 0) {
            return CN_MD_STATIC;
        }
        return CN_MD_LEFT;
    }
    if (delta_j > 0) {
        return CN_MD_UP_RIGHT;
    }
    if (delta_j == 0) {
        return CN_MD_UP;
    }
    return CN_MD_UP_LEFT;
}

int ISearch::direction(const Node &from, const Node &to) const {
    int delta_i = to.i - from.i;
    int delta_j = to.j - from.j;
    return direction(delta_i, delta_j);
}
