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
    if (!(parent.i == map.start_i && parent.j == map.start_j && parent.z == map.start_h) &&
            !(current.i == map.start_i && current.j == map.start_j && current.z == map.start_h) &&
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
    std::list<Node>::const_iterator section_start;
    for (auto It = hppath.List.begin(); It != --hppath.List.end(); ++It) {
        section_start = It;
        liner.append_line(*section_start, *(++section_start));
        if (section_start != --hppath.List.end()) {
            lppath.List.pop_back();
        }
    }
    sresult.lppath = &lppath; //здесь у sresult - указатель на константу.
}

void Theta::makePrimaryPath(Node curNode) {
    Node current = curNode;
    while (current.parent) {
        hppath.List.push_front(current);
        current = *current.parent;
    }
    hppath.List.push_front(current);
    sresult.hppath = &hppath;
}

