#include "theta.h"
#include <math.h>
#include <iostream>
Theta::~Theta()
{
}

bool Theta::grid(int i, int j, const Map &map)
{
    if (!map.CellOnGrid(i,j)|| (map.CellOnGrid(i,j)&& !map.CellIsTraversable(i,j))) return true;
    else return false;
}

bool Theta::lineOfSight(int parent_i, int parent_j,int current_i, int current_j, const Map &map)
{
  int x1=current_i;
  int y1=current_j;
  int x0=parent_i;
  int y0=parent_j;

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
              if (grid(x0+(s_x-1)/2,y0+(s_y-1)/2, map)) return false;

              y0+=s_y;
              f-=dx;
          }
          if (f!=0 && grid(x0+(s_x-1)/2,y0+(s_y-1)/2, map)) return false;
          if (dy==0 && grid(x0+(s_x-1)/2,y0, map) && grid(x0+(s_x-1)/2,y0-1, map)) return false;

          x0+=s_x;
      }
  }
  else
      while (y0!=y1)
      {
          f+=dx;
          if (f>=dy)
          {
              if(grid(x0+(s_x-1)/2,y0+(s_y-1)/2, map)) return false;

              x0+=s_x;
              f-=dy;
          }

          if (f!=0 && grid(x0+(s_x-1)/2,y0+(s_y-1)/2, map)) return false;
          if (dx==0 && grid(x0,y0+(s_y-1)/2, map) && grid(x0-1,y0+(s_y-1)/2, map)) return false;
          y0+=s_y;
      }
 return true;
}


Node Theta::resetParent(Node current, Node parent, const Map &map, const EnvironmentOptions &options )
{
    if ((parent.i==map.start_i && parent.j==map.start_j) || (current.i==map.start_i && current.j==map.start_j))
        return current;
    if (lineOfSight(parent.parent->i, parent.parent->j,current.i, current.j, map))
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

void Theta::makeSecondaryPath(const Map &map, Node curNode)
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

void Theta::makePrimaryPath(Node curNode)
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

