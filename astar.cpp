#include "astar.h"

#include <algorithm>
#include <math.h>

Astar::Astar() : ISearch() { }

Astar::Astar(double w, int BT, int SL) {
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
}

void Astar::recovery_primary_path(Node *finish, int start_i, int start_j) {
    auto path = new NodeList;
    auto point = finish;
    while (!(point->i == start_i && point->j == start_j)) {
        path->List.push_front(*point);
        point = point->parent;
    }
    path->List.push_front(*point);
    sresult.lppath = path;
}

void Astar::recovery_secondary_path(Node *finish, int start_i, int start_j, const EnvironmentOptions &options) {
    auto path = new NodeList;
    Node prev = *sresult.lppath->List.begin();
    auto it = sresult.lppath->List.begin();
    ++it;
    int dir = 0;
    for (; it != sresult.lppath->List.end(); ++it) {
        if (direction(prev, *it) != dir) {
            path->List.push_back(prev);
        }
        dir = direction(prev, *it);
        prev = *it;
    }
    path->List.push_back(prev);
    sresult.hppath = path;
}

Dijkstra::Dijkstra(double weight, int BT, int SL) {
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;
}

inline double Dijkstra::heuristic(int, int, int, int, const EnvironmentOptions &) const {
    return 0.0;
}
