#include "astar.h"

#include <algorithm>
#include <math.h>

Astar::Astar(double w, int BT, int SL)
{
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
}

Astar::~Astar()
{
}

double Astar::heuristic(int i, int j, int goal_i, int goal_j, const EnvironmentOptions &options) const {
    double result;
    int delta_i, delta_j;
    switch (options.metrictype) {
        case CN_SP_MT_CHEB:
            result = std::max(std::abs(i - goal_i), std::abs(j - goal_j));
            break;
        case CN_SP_MT_MANH:
            result = std::abs(i - goal_i) + std::abs(j - goal_j);
            break;
        case CN_SP_MT_DIAG:
            delta_i = std::abs(i - goal_i);
            delta_j = std::abs(j - goal_j);
            if (delta_i < delta_j) {
                result = options.diagonalcost * delta_i + options.linecost * (delta_j - delta_i);
            } else {
                result = options.diagonalcost * delta_j + options.linecost * (delta_i - delta_j);
            }
            break;
        case CN_SP_MT_EUCL:
        default:
            result = std::sqrt(std::pow(i - goal_i, 2) + std::pow(j - goal_j, 2));
    }
    return hweight * result;
}
