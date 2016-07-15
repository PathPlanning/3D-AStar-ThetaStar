#ifndef ASTAR_H
#define ASTAR_H
#include "gl_const.h"
#include "isearch.h"
#include "ilogger.h"
#include <stdlib.h>
#include <math.h>
class Astar : public ISearch
{
    public:
        Astar(double weight, int BT, int SL);

protected:
    virtual double heuristic(int i, int j, int goal_i, int goal_j, const EnvironmentOptions &options) const;
};

#endif
