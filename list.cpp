#include "list.h"


NodeList::NodeList(void)
{

}

NodeList::~NodeList(void)
{

}

bool NodeList::find(int x, int y)
{
    std::list<Node>::iterator iter = List.begin();
    for(; iter != List.end(); ++iter)
    {
        if((iter->i == x) && (iter->j == y))
        {
            return true;
        }
    }
    if((iter->i == x) && (iter->j == y))
    {
        return true;
    }
    return false;
}

std::list<Node>::iterator NodeList::find_i(int x, int y)
{
    std::list<Node>::iterator iter = List.begin();
    for(; iter != List.end(); ++iter)
    {
        if((iter->i == x) && (iter->j == y))
        {
            break;
        }
    }
    return iter;
}


