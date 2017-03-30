#ifndef BFS_H
#define BFS_H
#include "gl_const.h"
#include "isearch.h"
#include "astar.h"

// TODO write breadth-first search instead of best-first search algorithm here
class BFS : public Astar
{
protected:
    virtual void addOpen(Node newNode, uint_least32_t key);
    public:
        BFS();
        ~BFS(void);

};

#endif
