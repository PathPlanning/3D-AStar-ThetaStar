#ifndef ASTAR_H
#define ASTAR_H

#include "gl_const.h"
#include "isearch.h"
#include "ilogger.h"

class Astar : public ISearch {
public:
    Astar();

    Astar(double weight, int BT, int SL, int i);

    ~Astar();

protected:
    double computeHFromCellToCell(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h,
                                  const EnvironmentOptions &options);

};

#endif
