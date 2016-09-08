#include "bfs.h"

BFS::BFS(int i)
{
    open=new NodeList[i];
}

BFS::~BFS()
{
}

double BFS::computeHFromCellToCell(int start_i, int start_j, int fin_i, int fin_j, const EnvironmentOptions &options)
{
    return 0;
}

void BFS::addOpen(Node newNode)
{
    std::list<Node>::iterator iter=open[newNode.i].List.begin();

    while(iter != open[newNode.i].List.end() && ((newNode.i != iter->i)||(newNode.j != iter->j)))
    {
        ++iter;
    }

    if(iter!=open[newNode.i].List.end())
    {
        if(iter->g>newNode.g)
        {
            open[newNode.i].List.erase(iter);
        }
        else
            return;
    }

    open[newNode.i].List.push_back(newNode);
}
