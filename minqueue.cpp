#include "minqueue.h"

#include <limits>

void minqueue::const_iterator::generate_val() {
    val.coord = it->first;
    val.F = it->second.F;
    val.g = it->second.g;
    val.H = it->second.H;
    val.parent = it->second.parent;
}

minqueue::const_iterator::const_iterator(const typename std::map<vertex, croppedNode>::const_iterator &init) : it(init) {
    generate_val();
}

minqueue::const_iterator::const_iterator(const const_iterator& other) : it(other.it), val(other.val) {}

minqueue::const_iterator& minqueue::const_iterator::operator++() {
    ++it;
    generate_val();
    return *this;
}

minqueue::const_iterator minqueue::const_iterator::operator++(int) {
    const_iterator tmp(*this);
    operator++();
    return tmp;
}

bool minqueue::const_iterator::operator==(const const_iterator& rhs) const {return it == rhs.it;}

bool minqueue::const_iterator::operator!=(const const_iterator& rhs) const {return it != rhs.it;}

extNode minqueue::const_iterator::operator*() const {return val;}


minqueue::minqueue(int BT) : data() {
    if (BT == CN_SP_BT_GMIN) {
        bt_less = [](const croppedNode &x, const croppedNode &y) { return x.g < y.g;};
    } else { // process default case as g-max
        bt_less = [](const croppedNode &x, const croppedNode &y) { return x.g > y.g;};
    }
}

bool minqueue::less(const croppedNode &x, const croppedNode &y) const {
    if (x.F < y.F) {
        return true;
    }
    if (x.F == y.F) {
        bt_less(x, y);
    }
    return false;
}

extNode minqueue::top() const {
    return min;
}

minqueue::const_iterator minqueue::cbegin() const {
    return minqueue::const_iterator(data.cbegin());
}

minqueue::const_iterator minqueue::cend() const {
    return minqueue::const_iterator(data.cend());
}

void minqueue::pop() {
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

size_t minqueue::size() const {
    return data.size();
}

bool minqueue::empty() const {
    return data.empty();
}

void minqueue::push(extNode node) {
    if (data.find(node.coord) == data.end() || data[node.coord].g > node.g) {
        data[node.coord] = node;
        if (data.size() == 1 || less(node, min)) {
            min = node;
        }
    }
}

/*extNode minqueue::top() const {
    return *data.begin();
}

size_t minqueue::size() const {
    return _size;
}

bool minqueue::empty() const {
    return data.empty();
}

void minqueue::pop() {
    data.pop_front();
    --_size;
}

void minqueue::push(extNode node) {
    auto it_to_ins = data.end();
    auto found = data.end();
    auto prev = data.begin();
    for (auto it = data.begin(); it != data.end(); ++it) {
        if (it_to_ins == data.end() &&
                (it == data.begin() || less(*prev, node)) && !less(*it, node)) {
            it_to_ins = it;
        }
        if (it->coord.i == node.coord.i && it->coord.j == node.coord.j) {
            found = it;
        }
        if (it != data.begin()) {
            ++prev;
        }
    }

    if (found == data.end()) {
        data.insert(it_to_ins, node);
        ++_size;
    } else if (node.g < found->g) {
        if (found == it_to_ins) {
            *found = node;
        } else {
            data.erase(found);
            data.insert(it_to_ins, node);
        }
    }
}
*/

