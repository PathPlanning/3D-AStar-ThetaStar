#ifndef ASTAR_H
#define ASTAR_H

#include "gl_const.h"
#include "isearch.h"

class Astar : public ISearch {
public:
    Astar();
    Astar(double weight, int BT, int SL);

protected:
    virtual void recovery_primary_path(Node *finish, int start_i, int start_j) override;

    virtual void recovery_secondary_path(Node *finish, int start_i, int start_j, const EnvironmentOptions &options) override;
};

class Dijkstra : public Astar {
public:
    Dijkstra(double weight, int BT, int SL);

protected:
    virtual double heuristic(int, int, int, int, const EnvironmentOptions &) const override;
};

#endif
