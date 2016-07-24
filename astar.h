#ifndef ASTAR_H
#define ASTAR_H

#include "gl_const.h"
#include "isearch.h"
#include "minqueue.h"

class Astar : public ISearch {
public:
    Astar();
    Astar(double weight, int BT, int SL);

protected:
    virtual void add_successors_to_opened(const extNode &pos,
                                          minqueue &opened,
                                          const EnvironmentOptions &options, const Map &map);
};

class Dijkstra : public Astar {
public:
    Dijkstra(double weight, int BT, int SL);

protected:
    virtual double heuristic(int, int, int, int, const EnvironmentOptions &) const override;
};

#endif
