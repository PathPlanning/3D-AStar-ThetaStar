#include "dijkstra.h"

Dijkstra::Dijkstra(int i)
{
    open=new NodeList[i];
}

Dijkstra::~Dijkstra(void)
{
}

double Dijkstra::computeHFromCellToCell(int start_i, int start_j, int fin_i, int fin_j, const EnvironmentOptions &options)
{
    return 0;
}

void Dijkstra::addOpen(Node newNode)
{
    std::list<Node>::iterator iter,pos;

    bool posFound=false;

    pos = open[newNode.i].List.end();

    if (open[newNode.i].List.size()==0)
    {
        open[newNode.i].List.push_back(newNode);
        return;
    }

    for(iter=open[newNode.i].List.begin(); iter!=open[newNode.i].List.end(); ++iter)
    {
        if ((iter->g >= newNode.g) && (!posFound))
        {
            pos=iter;
            posFound=true;
        }

        if (((iter->i) == newNode.i) && (iter->j)==newNode.j)
        {
            if (newNode.g >= iter->g)
                    return;
            else
            {
                if(pos == iter)
                {	iter->i = newNode.i;
                        iter->j = newNode.j;
                        iter->F = newNode.F;
                        iter->g = newNode.g;
                        iter->parent = newNode.parent;
                        return;
                }
                open[newNode.i].List.erase(iter);
                break;
            }
        }
    }

    open[newNode.i].List.insert(pos,newNode);
}
