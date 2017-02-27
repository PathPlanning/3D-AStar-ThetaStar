#include "theta.h"
#include "Bresenham.h"
#include <math.h>
#include <iostream>

Theta::~Theta() {
}

double Theta::EuclidDistance(const Node &from, const Node &to, const EnvironmentOptions &options) const {
    return options.linecost * std::sqrt(
            (to.i - from.i) * (to.i - from.i) + (to.j - from.j) * (to.j - from.j) + (to.z - from.z) * (to.z - from.z));
}

Node Theta::resetParent(Node current, Node parent, const Map &map, const EnvironmentOptions &options) {
    if (current.parent->parent != nullptr &&
            (parent.parent->g + EuclidDistance(*(parent.parent), current, options) - current.g) < computation_eps
        && LineOfSight(map).line_of_sight(*(parent.parent), current)) {

        current.g = parent.parent->g +
                    EuclidDistance(*(parent.parent), current, options);
        current.parent = parent.parent;
    }
    return current;
}

void Theta::makeSecondaryPath(const Map &map, Node curNode) {
    Liner liner(map, &lppath.List);
    std::list<Node>::iterator cur;
    for (auto it = hppath.List.begin(); it != --hppath.List.end();) {
        if (!lppath.List.empty()) {
            lppath.List.pop_back();
        }
        cur = it;
        liner.append_line(*cur, *(++it));
    }
    sresult.lppath = &lppath;
}

void Theta::makePrimaryPath(Node curNode) {
    Node current = curNode;
    while (current.parent != nullptr) {
        hppath.List.push_front(current);
        current = *current.parent;
    }
    hppath.List.push_front(current);
    sresult.hppath = &hppath;
}

