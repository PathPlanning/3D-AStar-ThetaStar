#ifndef ASEARCH_CUTTED_MINQUEUE_H
#define ASEARCH_CUTTED_MINQUEUE_H

#include <map>
#include <functional>
#include <iostream>

#include "node.h"
#include "gl_const.h"

/*
 * Priority queue for opened list.
 * Made on map structure.
 * O(n) - extract minimum.
 * O(log n) - insert/update node.
 */
class minqueue {
private:
    std::map<vertex, croppedNode> data;
    std::function<bool(const croppedNode&, const croppedNode&)> bt_less;
    extNode min;

    bool less(const croppedNode &x, const croppedNode &y) const;

public:
    minqueue(int BT = CN_SP_BT_GMAX);

    /*
     * Insert new node. Or updates node with the same coordinates
     */
    void push(extNode);

    extNode top() const;

    size_t size() const;

    bool empty() const;

    void pop();
};


#endif //ASEARCH_CUTTED_MINQUEUE_H
