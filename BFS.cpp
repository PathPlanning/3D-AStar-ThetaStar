#include "BFS.h"

BFS::BFS(double weight, int BT, int SL) {
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;
}

std::list<Node> BFS::find_successors(Node *current, const Map &map, const EnvironmentOptions &options) const {
    std::list<Node> result = ISearch::find_successors(current, map, options);
    for (auto it = result.begin(); it != result.end(); ++it) {
        it->F = it->H;
    }
    return result;
}