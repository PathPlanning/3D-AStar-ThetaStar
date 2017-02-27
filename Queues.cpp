#include "Queues.h"
#include "node.h"
#include "gl_const.h"

bool iOpen::less(const Node &x, const Node &y) const {
    if (x.F == y.F) {
        switch (breaking_tie) {
            case CN_SP_BT_GMIN:
                return x.g < y.g;
            case CN_SP_BT_GMAX:
            default:
                return x.g > y.g;
        }
    }
    return x.F < y.F;
}

SortedList::SortedList(size_t size, int breakingtie) : data(size), size_(0), min_pos(size) {
    breaking_tie = breakingtie;
}

size_t SortedList::size() const {
    return size_;
}

bool SortedList::empty() const {
    return (size_ == 0);
}

bool SortedList::Insert(const Node &newNode) {
    std::list<Node>::iterator iter, pos;

    bool posFound = false;

    pos = data[newNode.i].end();

    if (data[newNode.i].empty()) {
        data[newNode.i].push_back(newNode);
        ++size_;
        return true;
    }

    for (iter = data[newNode.i].begin(); iter != data[newNode.i].end(); ++iter) {
        if (!posFound && !less(*iter, newNode)) {
            pos = iter;
            posFound = true;
        }

        if (iter->j == newNode.j && iter->z == newNode.z) {
            if (newNode.F >= iter->F) {
                return false;
            } else {
                if (pos == iter) {
                    iter->g = newNode.g;
                    iter->F = newNode.F;
                    iter->H = newNode.H;
                    return true;
                }
                data[newNode.i].erase(iter);
                --size_;
                break;
            }
        }
    }
    ++size_;
    data[newNode.i].insert(pos, newNode);
    return true;
}

Node SortedList::FindMin() const {
    Node min;
    min.F = -1;
    for (size_t i = 0; i < data.size(); i++) {
        if (!data[i].empty())
            if (min.F == -1 || less(*data[i].begin(), min)) {
                min = *data[i].begin();
                min_pos = i;
            }
    }
    return min;
}

void SortedList::DeleteMin() {
    if (min_pos >= data.size()) {
        FindMin();
    }
    data[min_pos].pop_front();
    --size_;
    min_pos = data.size();
}

ClusteredSets::ClusteredSets(size_t size, int breakingtie) : loc_mins(size), data(size), size_(0), min_pos(size) {
    breaking_tie = breakingtie;
}

size_t ClusteredSets::size() const {
    return size_;
}

bool ClusteredSets::empty() const {
    return (size_ == 0);
}

bool ClusteredSets::Insert(const Node &NewNode) {
    bool node_found = false;
    bool updated = false;

    auto pos = data[NewNode.i].find(NewNode);

    if (pos != data[NewNode.i].end()) {
        node_found = true;
        if (NewNode.g < pos->g) {
            data[NewNode.i].erase(pos);
            data[NewNode.i].insert(NewNode);
            updated = true;
        } else {
            return false;
        }
    }

    if (!node_found) {
        updated = true;
        data[NewNode.i].insert(NewNode);
        ++size_;
    }
    if (data[NewNode.i].size() == 1 || less(NewNode, loc_mins[NewNode.i])) {
        loc_mins[NewNode.i] = NewNode;
    }
    return updated;
}

Node ClusteredSets::FindMin() const {
    for (min_pos = 0; data[min_pos].empty(); ++min_pos) {}
    for (size_t i = min_pos + 1; i < loc_mins.size(); ++i) {
        if (!data[i].empty() && less(loc_mins[i], loc_mins[min_pos])) {
            min_pos = i;
        }
    }
    return loc_mins[min_pos];
}

void ClusteredSets::DeleteMin() {
    if (min_pos == loc_mins.size()) {
        FindMin();
    }

    data[min_pos].erase(loc_mins[min_pos]);
    --size_;
    Node min;
    if (!data[min_pos].empty()) {
        auto it = data[min_pos].begin();
        min = *(it++);
        for (; it != data[min_pos].end(); ++it) {
            if (less(*it, min)) {
                min = *it;
            }
        }

        loc_mins[min_pos] = min;
    }
    min_pos = loc_mins.size();
}
