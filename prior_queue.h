#ifndef ASEARCH_CUTTED_MINQUEUE_H
#define ASEARCH_CUTTED_MINQUEUE_H

#include <map>
#include <iterator>
#include <functional>
#include <iostream>
#include <list>
#include <vector>

#include "node.h"
#include "gl_const.h"

class IQueue {
protected:
    int breakingtie;
    size_t _size;

    virtual bool less(const Node &x, const Node &y) const;

public:
    virtual size_t size() const;

    virtual bool empty() const;

    IQueue(int);
};

/*
 * Priority queue for opened list.
 * Made on map structure.
 * O(n) - extract minimum.
 * O(log n) - insert/update node.
 */
class map_queue : public IQueue {
private:
    std::map<vertex, croppedNode> data;
    extNode min;

    bool less(const croppedNode& x, const croppedNode& y) const;

public:
    class const_iterator : public std::iterator<std::forward_iterator_tag, extNode> {
    private:
        typename std::map<vertex, croppedNode>::const_iterator it;
        extNode val;
        friend class map_queue;
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

    map_queue(int BT);

    /*
     * Insert new node. Or updates node with the same coordinates
     */
    bool push(const extNode&);

    const_iterator cbegin() const;

    const_iterator cend() const;

    extNode top() const;

    virtual size_t size() const override;

    virtual bool empty() const override;

    void pop();
};

class coord_list_queue : public IQueue {
private:
    std::vector<std::list<Node>> data;
    mutable size_t cached_min;

    void CacheMinimum() const;
public:
    coord_list_queue(int BT = CN_SP_BT_GMAX, int map_height=1);

    /*
     * Insert new node. Or updates node with the same coordinates
     */
    bool push(const Node&);

    Node top() const;

    void pop();
};


#endif //ASEARCH_CUTTED_MINQUEUE_H
