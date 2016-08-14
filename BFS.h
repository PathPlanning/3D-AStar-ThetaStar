#ifndef HEURISTICSEARCH_BFS_H
#define HEURISTICSEARCH_BFS_H


#include "astar.h"

class BFS : public Astar {
public:
    BFS(double weight, int BT, int SL);

protected:
    virtual std::list<Node> find_successors(Node *current, const Map &map, const EnvironmentOptions &options) const;
};


#endif //HEURISTICSEARCH_BFS_H
