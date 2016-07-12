#ifndef LIST_H
#define LIST_H
#include "node.h"
#include <list>

class NodeList
{
    public:
        NodeList(void);
        ~NodeList(void);

        bool find(int x, int y);
        std::list<Node>::iterator find_i(int x, int y);

        std::list<Node> List;
};
#endif
