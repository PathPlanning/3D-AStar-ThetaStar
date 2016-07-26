#ifndef ASEARCH_CUTTED_MINQUEUE_H
#define ASEARCH_CUTTED_MINQUEUE_H

#include <map>
#include <iterator>
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
    class const_iterator : public std::iterator<std::forward_iterator_tag, extNode> {
    private:
        typename std::map<vertex, croppedNode>::const_iterator it;
        extNode val;
        friend class minqueue;
        void generate_val();
        const_iterator(const typename std::map<vertex, croppedNode>::const_iterator&);
    public:
        const_iterator(const const_iterator&);
        const_iterator& operator++();
        const_iterator operator++(int);
        bool operator==(const const_iterator&) const;
        bool operator!=(const const_iterator&) const;
        extNode operator*() const;
    };

    minqueue(int BT = CN_SP_BT_GMAX);

    /*
     * Insert new node. Or updates node with the same coordinates
     */
    bool push(const extNode&);

    const_iterator cbegin() const;

    const_iterator cend() const;

    extNode top() const;

    size_t size() const;

    bool empty() const;

    void pop();
};


#endif //ASEARCH_CUTTED_MINQUEUE_H
