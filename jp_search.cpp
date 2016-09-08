#include "jp_search.h"
#include <stdlib.h>
#include "iostream"
#include <math.h>
#ifdef __linux__
    #include <sys/time.h>
#else
    #include <windows.h>
#endif

JP_Search::~JP_Search()
{
}

void JP_Search::findJP(int move_i, int move_j, Node curNode, const Map &map, std::list<Node> &successors, const EnvironmentOptions &options)
{
    bool findOK = false;
    while(!findOK)
    {
        if(map.CellOnGrid(curNode.i+move_i, curNode.j+move_j))
        {
            if(options.allowsqueeze == CN_SP_AS_FALSE)
                if(move_i != 0 && move_j != 0)
                    if(!map.CellIsTraversable(curNode.i, curNode.j+move_j) && !map.CellIsTraversable(curNode.i+move_i, curNode.j))
                        return;
            if(map.CellIsTraversable(curNode.i+move_i, curNode.j+move_j))
            {
                curNode.i += move_i;
                curNode.j += move_j;
                curNode.g = curNode.g+MoveCost(curNode.i, curNode.j, curNode.i+move_i, curNode.j+move_j, options);
            }
            else
                return;//наткнулись на препятствие или вышли за границы карты
        }
        else return;
        if(map.goal_i == curNode.i && map.goal_j == curNode.j)//если дошли до целевой вершины
            findOK = true;
        if(options.allowdiagonal == CN_SP_AD_TRUE)//если движение по диагонали разрешено
        {
            if(move_i == 0 && map.CellOnGrid(curNode.i, curNode.j+move_j))//если движемся по j
            {
                if(map.CellOnGrid(curNode.i+1, curNode.j))//проверка на границы
                    if(map.CellIsTraversable(curNode.i+1, curNode.j+move_j) && map.CellIsObstacle(curNode.i+1, curNode.j))//проверка на вынужденных соседей
                        findOK = true;
                if(map.CellOnGrid(curNode.i-1,curNode.j))
                    if(map.CellIsTraversable(curNode.i-1, curNode.j+move_j) && map.CellIsObstacle(curNode.i-1, curNode.j))//с обеих сторон
                        findOK = true;
            }
            if(move_j == 0 && map.CellOnGrid(curNode.i+move_i, curNode.j))//если движемся по i
            {
                if(map.CellOnGrid(curNode.i, curNode.j+1))
                    if(map.CellIsTraversable(curNode.i+move_i, curNode.j+1) && map.CellIsObstacle(curNode.i, curNode.j+1))//проверка на вынужденных соседей
                        findOK = true;
                if(map.CellOnGrid(curNode.i, curNode.j-1))
                    if(map.CellIsTraversable(curNode.i+move_i, curNode.j-1) && map.CellIsObstacle(curNode.i, curNode.j-1))//с обеих сторон
                        findOK = true;
            }
            if(move_i != 0 && move_j != 0)//если движемся по диагонали
            {
                if(map.CellOnGrid(curNode.i-move_i, curNode.j+move_j))
                    if(map.CellIsObstacle(curNode.i-move_i, curNode.j) && map.CellIsTraversable(curNode.i-move_i, curNode.j+move_j))//проверка на вынужденных соседей
                        findOK = true;
                if(!findOK && map.CellOnGrid(curNode.i+move_i, curNode.j-move_j))
                    if(map.CellIsObstacle(curNode.i, curNode.j-move_j) && map.CellIsTraversable(curNode.i+move_i, curNode.j-move_j))//проверка на вынужденных соседей с другой стороны
                        findOK = true;
                if(!findOK)
                    if(findNeighbors(move_i, 0, curNode, map))//проверка на вынужденного соседа по i
                        findOK = true;
                if(!findOK)
                    if(findNeighbors(0, move_j, curNode, map))//проверка на вынужденного соседа по j
                        findOK = true;
            }
        }
        else//если движение по диагонали запрещено
        {
            if(!findOK)
                if(findNeighbors(move_j, move_i, curNode, map))
                    findOK = true;
            if(!findOK)
                if(findNeighbors(-move_j, -move_i, curNode, map))
                    findOK = true;
        }
    }
    if(findOK && close.find(curNode)==close.end())
    {
        successors.push_front(curNode);
        return;
    }
}

bool JP_Search::findNeighbors(int move_i, int move_j, Node curNode, const Map &map)
{
    while(map.CellOnGrid(curNode.i, curNode.j) && map.CellIsTraversable(curNode.i, curNode.j))
    {
        if(map.goal_i == curNode.i && map.goal_j == curNode.j)//если дошли до целевой вершины
            return true;
        if(move_i == 0 && map.CellOnGrid(curNode.i, curNode.j+move_j))//если движемся по j
        {
            if(map.CellOnGrid(curNode.i+1, curNode.j))//проверка на границы
                if(map.CellIsTraversable(curNode.i+1, curNode.j+move_j) && map.CellIsObstacle(curNode.i+1, curNode.j))//проверка на вынужденных соседей
                   return true;
             if(map.CellOnGrid(curNode.i-1,curNode.j))
                 if(map.CellIsTraversable(curNode.i-1, curNode.j+move_j) && map.CellIsObstacle(curNode.i-1, curNode.j))//с обеих сторон
                   return true;
         }
         if(move_j == 0 && map.CellOnGrid(curNode.i+move_i, curNode.j))//если движемся по i
         {
             if(map.CellOnGrid(curNode.i, curNode.j+1))
                 if(map.CellIsTraversable(curNode.i+move_i, curNode.j+1) && map.CellIsObstacle(curNode.i, curNode.j+1))
                     return true;
             if(map.CellOnGrid(curNode.i, curNode.j-1))
                 if(map.CellIsTraversable(curNode.i+move_i, curNode.j-1) && map.CellIsObstacle(curNode.i, curNode.j-1))
                     return true;
         }
         curNode.i += move_i;
         curNode.j += move_j;
    }
    return false;
}



int JP_Search::findDirection(int current_i, int parent_i)//определяем направление
{
    if(current_i < parent_i)
        return -1;
    else if(current_i > parent_i)
        return 1;
    else
        return 0;
}

std::list<Node> JP_Search::findSuccessors(Node curNode, const Map &map, const EnvironmentOptions &options)
{
    int move_i = 0, move_j = 0;
    std::list<Node> successors;

    if(options.allowdiagonal == CN_SP_AD_TRUE)
    {
        if(curNode.i == map.start_i && curNode.j == map.start_j)
            for(int n=-1; n<=1; n++)//цикл по всем соседним вершинам
                for(int m=-1; m<=1; m++)
                    if(n != 0 || m != 0)
                        findJP(n, m, curNode, map, successors, options);
        if(curNode.i != map.start_i || curNode.j != map.start_j)
        {
            move_i = findDirection(curNode.i, curNode.parent->i);
            move_j = findDirection(curNode.j, curNode.parent->j);

            if(move_i != 0 && move_j != 0)//если прыжковая точка найдена по диагонали
            {
                if(map.CellIsObstacle(curNode.i-move_i, curNode.j))
                    findJP(-move_i, move_j, curNode, map, successors, options);
                if(map.CellIsObstacle(curNode.i, curNode.j-move_j))
                    findJP(move_i, -move_j, curNode, map, successors, options);
                findJP(move_i, 0, curNode, map, successors, options);//ищем прыжковые точки по вертикали
                findJP(0, move_j, curNode, map, successors, options);//и горизонтали
            }
            findJP(move_i, move_j, curNode, map, successors, options);//поиск прыжковой точки в том же направлении
            if(move_i == 0)//если прыжковая точка найдена по горизонтали
            {
                if(map.CellOnGrid(curNode.i-move_j, curNode.j))
                    if(map.CellIsObstacle(curNode.i-move_j, curNode.j))
                        findJP(-move_j, move_j, curNode, map, successors, options);//поиск прыжковой точки по диагонали
                if(map.CellOnGrid(curNode.i+move_j, curNode.j))
                    if(map.CellIsObstacle(curNode.i+move_j, curNode.j))
                        findJP(move_j, move_j, curNode, map, successors, options);//в двух направлениях
            }
            else if(move_j==0)//то же самое, только по вертикали
            {
                if(map.CellOnGrid(curNode.i, curNode.j-move_i))
                    if(map.CellIsObstacle(curNode.i, curNode.j-move_i))
                        findJP(move_i, -move_i, curNode, map, successors, options);
                if(map.CellOnGrid(curNode.i, curNode.j+move_i))
                    if(map.CellIsObstacle(curNode.i, curNode.j+move_i))
                        findJP(move_i, move_i, curNode, map, successors, options);
            }
        }
    }
    else //если движение по диагонали запрещено
    {
        if(curNode.i == map.start_i && curNode.j == map.start_j)
            for(int n=-1; n<=1; n++)
                for(int m=-1; m<=1; m++)
                    if((n != 0 && m == 0) || (n == 0 && m != 0))
                        findJP(n, m, curNode, map, successors, options);
        if(curNode.i != map.start_i || curNode.j != map.start_j)
        {
            move_i=findDirection(curNode.i, curNode.parent->i);
            move_j=findDirection(curNode.j, curNode.parent->j);

            findJP(move_i, move_j, curNode, map, successors, options);
            findJP(move_j, move_i, curNode, map, successors, options);
            findJP(-move_j, -move_i, curNode, map, successors, options);
        }
    }
    return successors;
}

void JP_Search::makePrimaryPath(Node curNode)
{
    Node current=curNode;
    while(current.parent)
    {
        hppath.List.push_front(current);
        current=*current.parent;
    }
    hppath.List.push_front(current);
    sresult.hppath = &hppath;
}

void JP_Search::makeSecondaryPath(const Map &map, Node curNode)
{
    Node *forpath=&*hppath.List.begin();
    std::list<Node>::iterator iter = hppath.List.begin();
    iter++;
    Node *forpath2=&*iter;
    int x0,x1,y0,y1,s_x,s_y,dx,dy,f;
    Node inpath;

    lppath.List.push_back(*forpath);
    while(forpath2!=&*hppath.List.end())
    {
          x0=forpath->i;
          y0=forpath->j;
          x1=forpath2->i;
          y1=forpath2->j;
          dx= x1-x0;
          dy= y1-y0;
          f=0;

          if (dy<0) {dy=-dy; s_y=-1;}
          else s_y=1;

          if (dx<0) {dx=-dx; s_x=-1;}
          else s_x=1;

          if (dx>=dy)
          {
          while (x0!=x1)
            {
                f+=dy;
                if (f>=dx)
                {
                    y0+=s_y;
                    f-=dx;
                }
                x0+=s_x;
                inpath.i=x0;
                inpath.j=y0;
                lppath.List.push_back(inpath);
             }
           }
           else
           while (y0!=y1)
           {
            f+=dx;
            if (f>=dy)
            {
                x0+=s_x;
                f-=dy;
            }

            y0+=s_y;
            inpath.i=x0;
            inpath.j=y0;
            lppath.List.push_back(inpath);
            }
              forpath=forpath2;
              iter++;
              forpath2=&*iter;
    }
    sresult.lppath = &lppath; //здесь у sresult - указатель на константу.
}

Node JP_Search::resetParent(Node current, Node parent, const Map &map, const EnvironmentOptions &options )
{
    if(options.useresetparent==false)
        return current;
    if (((parent.i==map.start_i) && (parent.j==map.start_j)) ||(((current.i==map.start_i) && (current.j==map.start_j))))
        return current;

    bool ok=true;
    int x1=parent.parent->i;
    int y1=parent.parent->j;
    int x0=current.i;
    int y0=current.j;

    int dx= x1-x0;
    int dy= y1-y0;
    int s_x, s_y;
    int f=0;

    if (dy<0) {dy=-dy; s_y=-1;}
    else s_y=1;
    if (dx<0) {dx=-dx; s_x=-1;}
    else s_x=1;
    if (dx>=dy)
    {
        while (x0!=x1)
        {
            f+=dy;
            if (f>=dx)
            {
                if(map.getValue(x0+(s_x-1)/2,y0+(s_y-1)/2)) ok=false;
                y0+=s_y;
                f-=dx;
            }
            if (f!=0 && map.getValue(x0+(s_x-1)/2,y0+(s_y-1)/2)) ok=false;
            if (dy==0 && map.getValue(x0+(s_x-1)/2,y0) && map.getValue(x0+(s_x-1)/2,y0-1)) ok=false;

            x0+=s_x;
        }
    }
    else
        while (y0!=y1)
        {
            f+=dx;
            if (f>=dy)
            {
                if(map.getValue(x0+(s_x-1)/2,y0+(s_y-1)/2)) ok=false;

                x0+=s_x;
                f-=dy;
            }

            if (f!=0 && map.getValue(x0+(s_x-1)/2,y0+(s_y-1)/2)) ok=false;
            if (dx==0 && map.getValue(x0,y0+(s_y-1)/2) && map.getValue(x0-1,y0+(s_y-1)/2)) ok=false;
            y0+=s_y;
        }

    if (ok)
    {
        if ((parent.parent->g + computeHFromCellToCell(parent.parent->i,parent.parent->j, current.i, current.j, options)-current.g )<0.0001)
        {
            current.g=parent.parent->g +computeHFromCellToCell(parent.parent->i,parent.parent->j, current.i, current.j, options);
            current.parent=parent.parent;
            return current;
        }
    }
    return current;
}
