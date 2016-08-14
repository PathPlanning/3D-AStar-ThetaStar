#include "Theta_star.h"

#include <math.h>

Theta_star::Theta_star(double weight, int BT, int SL) {
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;
}

double Theta_star::euclid_distance(const vertex &from, const vertex &to, double linecost) const {
    return linecost * std::sqrt(std::pow(from.i - to.i, 2) + std::pow(from.j - to.j, 2));
}

double Theta_star::euclid_distance(const Node &from, const Node &to, double linecost) const {
    return linecost * std::sqrt(std::pow(from.i - to.i, 2) + std::pow(from.j - to.j, 2));
}

bool Theta_star::line_of_sight(const Node &from, const Node &to, const Map &map) const {
    int delta_i = std::abs(from.i - to.i);
    int delta_j = std::abs(from.j - to.j);
    int step_i = (from.i < to.i ? 1 : -1);
    int step_j = (from.j < to.j ? 1 : -1);
    int error = 0;
    int i = from.i;
    int j = from.j;
    if (delta_i > delta_j) {
        for (; i != to.i; i += step_i) {
            if (map.CellOnGrid(i, j) && map.CellIsObstacle(i, j)) {
                return false;
            }
            error += delta_j;
            if ((error << 1) > delta_i) {
                j += step_j;
                error -= delta_i;
            }
        }
    } else {
        for (; j != to.j; j += step_j) {
            if (map.CellOnGrid(i, j) && map.CellIsObstacle(i, j)) {
                return false;
            }
            error += delta_i;
            if ((error << 1) > delta_j) {
                j += step_j;
                error -= delta_j;
            }
        }
    }
    return true;
}

void
Theta_star::construct_line(const Node &from, const Node &to, NodeList &out, const EnvironmentOptions &options) const {
    int delta_i = std::abs(from.i - to.i);
    int delta_j = std::abs(from.j - to.j);
    int step_i = (from.i < to.i ? 1 : -1);
    int step_j = (from.j < to.j ? 1 : -1);
    int error = 0;
    int i = from.i;
    int j = from.j;
    Node current;
    if (delta_i > delta_j) {
        for (; i != to.i; i += step_i) {
            current.i = i;
            current.j = j;
            current.g = out.List.rbegin()->g + euclid_distance(*out.List.rbegin(), current, options.linecost);
            out.List.push_back(current);
            error += delta_j;
            if ((error << 1) > delta_i) {
                j += step_j;
                error -= delta_i;
            }
        }
    } else {
        for (; j != to.j; j += step_j) {
            current.i = i;
            current.j = j;
            current.g = out.List.rbegin()->g + euclid_distance(*out.List.rbegin(), current, options.linecost);
            out.List.push_back(current);
            error += delta_i;
            if ((error << 1) > delta_j) {
                j += step_j;
                error -= delta_j;
            }
        }
    }
}

void Theta_star::improve_parent(Node *current, const Map &map, const EnvironmentOptions &options) const {
    Node *potential_parent = current->parent->parent;
    if (euclid_distance(*potential_parent, *current, options.linecost) < current->g &&
        line_of_sight(*potential_parent, *current, map)) {
        current->g = euclid_distance(*potential_parent, *current, options.linecost);
        current->F = current->g + current->H;
        current->parent = potential_parent;
    }
}

void Theta_star::recovery_primary_path(Node *finish, int start_i, int start_j) {
    auto path = new NodeList;
    auto point = finish;
    while (!(point->i == start_i && point->j == start_j)) {
        path->List.push_front(*point);
        point = point->parent;
    }
    path->List.push_front(*point);
    sresult.hppath = path;
}

void Theta_star::recovery_secondary_path(Node *finish, int start_i, int start_j, const EnvironmentOptions &options) {
    auto path = new NodeList;
    Node prev = *sresult.hppath->List.begin();
    auto it = sresult.hppath->List.begin();
    ++it;
    for (; it != sresult.hppath->List.end(); ++it) {
        construct_line(prev, *it, *path, options);
        prev = *it;
    }
    prev.parent = &(*(path->List.rbegin()));
    path->List.push_back(prev);
    sresult.lppath = path;
}