#ifndef HEURISTICSEARCH_BFS_H
#define HEURISTICSEARCH_BFS_H


#include "isearch.h"

class BFS : public ISearch {
public:
    BFS(double weight, int BT, int SL);

protected:
    virtual void add_successors_to_opened(const extNode &pos,
                                          minqueue &opened,
                                          const std::map<vertex, std::pair<double, vertex>> &,
                                          const EnvironmentOptions &options, const Map &map);
};


#endif //HEURISTICSEARCH_BFS_H
