#include "prior_queue.h"

#include <limits>


//Common class
IQueue::IQueue(int BT = CN_SP_BT_GMAX) : breakingtie(BT), _size(0) {}

bool IQueue::less(const Node &x, const Node &y) const {
    if (x.F < y.F) {
        return true;
    }
    if (x.F == y.F) {
        if (breakingtie == CN_SP_BT_GMIN) {
            return x.g < y.g;
        } else {
            return x.g > y.g;
        }
    }
    return false;
}

size_t IQueue::size() const {
    return _size;
}

bool IQueue::empty() const {
    return _size == 0;
}


//Impl. on map
map_queue::map_queue(int BT) : IQueue(BT), data() {}

bool map_queue::less(const croppedNode &x, const croppedNode &y) const {
    if (x.F < y.F) {
        return true;
    }
    if (x.F == y.F) {
        if (breakingtie == CN_SP_BT_GMIN) {
            return x.g < y.g;
        } else {
            return x.g > y.g;
        }
    }
    return false;
}

extNode map_queue::top() const {
    return min;
}

map_queue::const_iterator map_queue::cbegin() const {
    return map_queue::const_iterator(data.cbegin());
}

map_queue::const_iterator map_queue::cend() const {
    return map_queue::const_iterator(data.cend());
}

void map_queue::pop() {
    data.erase(min.coord);
    min.F = std::numeric_limits<double>::infinity();
    for (auto it = data.begin(); it != data.end(); ++it) {
        if (less(it->second, min)) {
            min.coord = it->first;
            min.F = it->second.F;
            min.g = it->second.g;
            min.H = it->second.H;
            min.parent = it->second.parent;
        }
    }
}

size_t map_queue::size() const {
    return data.size();
}

bool map_queue::empty() const {
    return data.empty();
}

bool map_queue::push(const extNode &node) {
    if (data.find(node.coord) == data.end() || data[node.coord].g > node.g) {
        data[node.coord] = node;
        if (data.size() == 1 || less(node, min)) {
            min = node;
        }
        return true;
    }
    return false;
}

// Iterator
void map_queue::const_iterator::generate_val() {
    val.coord = it->first;
    val.F = it->second.F;
    val.g = it->second.g;
    val.H = it->second.H;
    val.parent = it->second.parent;
}

map_queue::const_iterator::const_iterator(const typename std::map<vertex, croppedNode>::const_iterator &init) : it(
        init) {
    generate_val();
}

map_queue::const_iterator::const_iterator(const const_iterator &other) : it(other.it), val(other.val) {}

map_queue::const_iterator &map_queue::const_iterator::operator++() {
    ++it;
    generate_val();
    return *this;
}

map_queue::const_iterator map_queue::const_iterator::operator++(int) {
    const_iterator tmp(*this);
    operator++();
    return tmp;
}

bool map_queue::const_iterator::operator==(const const_iterator &rhs) const { return it == rhs.it; }

bool map_queue::const_iterator::operator!=(const const_iterator &rhs) const { return it != rhs.it; }

extNode map_queue::const_iterator::operator*() const { return val; }


//Coord list impl.
coord_list_queue::coord_list_queue(int BT, int map_height) :
        IQueue(BT),
        data(map_height, std::list<Node>()),
        cached_min(map_height) {}

Node coord_list_queue::top() const {
    if (cached_min == data.size()) {
        CacheMinimum();
    }
    return *data[cached_min].begin();
}

void coord_list_queue::pop() {
    if (cached_min == data.size()) {
        CacheMinimum();
    }
    data[cached_min].pop_front();
    --_size;
    CacheMinimum();
}

bool coord_list_queue::push(const Node &node) {
    size_t pos = node.i % data.size();
    auto begin = data[pos].begin();
    auto end = data[pos].end();
    auto it_to_ins = end;
    auto found = end;
    auto prev = begin;
    for (auto it = begin; it != end; ++it) {
        if (it_to_ins == end &&
            (it == begin || less(*prev, node)) && !less(*it, node)) {
            it_to_ins = it;
        }
        if (it->j == node.j && it->i == node.i) {
            found = it;
        }
        if (it != begin) {
            ++prev;
        }
    }

    bool inserted = false;
    if (found == end) {
        data[pos].insert(it_to_ins, node);
        ++_size;
        inserted = true;
    } else if (node.g < found->g) {
        if (found == it_to_ins) {
            *found = node;
        } else {
            data[pos].erase(found);
            data[pos].insert(it_to_ins, node);
        }
        inserted = true;
    }
    if (inserted && cached_min != data.size() && less(node, *data[cached_min].begin())) {
        cached_min = pos;
    }
    return inserted;
}

void coord_list_queue::CacheMinimum() const {
    Node tmp;
    tmp.F = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < data.size(); ++i) {
        if (!data[i].empty() && less(*(data[i].begin()), tmp)) {
            tmp = *(data[i].begin());
            cached_min = i;
        }
    }
}

