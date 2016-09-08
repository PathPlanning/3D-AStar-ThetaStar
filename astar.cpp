#include "astar.h"
#include <stdlib.h>
#include <math.h>
#include "isearch.h"
#include <iostream>
Astar::Astar(double w, int BT, int SL, int i)
{
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
}

Astar::~Astar()
{
}

double Astar::computeHFromCellToCell(int start_i, int start_j, int fin_i, int fin_j, const EnvironmentOptions &options)
{
    #ifdef __linux__
        if (options.metrictype == CN_SP_MT_MANH)
                return options.linecost * (abs(fin_i - start_i) + abs(fin_j - start_j));
        if (options.metrictype == CN_SP_MT_CHEB)
                return options.linecost * std::max(abs(fin_i - start_i),abs(fin_j - start_j));
        if (options.metrictype == CN_SP_MT_EUCL)
                return options.linecost * (sqrt((fin_i - start_i)*(fin_i - start_i)+(fin_j - start_j)*(fin_j - start_j)));
        if (options.metrictype == CN_SP_MT_DIAG)
                return (options.linecost * abs(abs(fin_i - start_i) - abs(fin_j - start_j)) + options.diagonalcost * (std::min(abs(fin_i - start_i),abs(fin_j - start_j))));
    #else
        if (options.metrictype == CN_SP_MT_MANH)
                return options.linecost * (abs(fin_i - start_i) + abs(fin_j - start_j));
        if (options.metrictype == CN_SP_MT_CHEB)
                return options.linecost * std::max(abs(fin_i - start_i),abs(fin_j - start_j));
        if (options.metrictype == CN_SP_MT_EUCL)
                return options.linecost * (sqrt((fin_i - start_i)*(fin_i - start_i)+(fin_j - start_j)*(fin_j - start_j)));
        if (options.metrictype == CN_SP_MT_DIAG)
                return (options.linecost * abs(abs(fin_i - start_i) - abs(fin_j - start_j)) + options.diagonalcost * (std::min(abs(fin_i - start_i),abs(fin_j - start_j))));
    #endif
    return 0;
}

void Astar::addOpen(Node newNode)
{
    std::list<Node>::iterator iter,pos;

    bool posFound=false;

    pos = open[newNode.i].List.end();

    if (open[newNode.i].List.size() == 0)
    {
        open[newNode.i].List.push_back(newNode);
        openSize++;
        return;
    }

    for(iter=open[newNode.i].List.begin(); iter != open[newNode.i].List.end(); ++iter)
    {
        if ((iter->F >= newNode.F) && (!posFound))
        {
            if (iter->F == newNode.F)
            {
                switch(breakingties)
                {
                    case CN_SP_BT_GMAX:
                    {
                        if (newNode.g >= iter->g)
                        {
                            pos=iter;
                            posFound=true;
                        }

                    }
                    case CN_SP_BT_GMIN:
                    {
                        if (newNode.g <= iter->g)
                        {
                            pos=iter;
                            posFound=true;
                        }
                    }
                }
            }
            else
            {
                pos=iter;
                posFound=true;
            }
        }

        if (((iter->i) == newNode.i) && (iter->j)==newNode.j)
        {
            if (newNode.F >= iter->F)
            {
                return;
            }
            else
            {
                if(pos == iter)
                {
                    iter->F = newNode.F;
                    iter->g = newNode.g;
                    iter->parent = newNode.parent;
                    return;
                }
                open[newNode.i].List.erase(iter);
                openSize--;
                break;
            }
        }
    }
    openSize++;
    open[newNode.i].List.insert(pos,newNode);
}
