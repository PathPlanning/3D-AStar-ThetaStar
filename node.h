#ifndef NODE_H
#define NODE_H

#include <functional>

struct Node
{
    int i, j;
    double F, g, H;
    Node *parent;

    bool operator<(const Node& other) const {
        if (i < other.i) {
            return true;
        } else if (i == other.i) {
            return j < other.j;
        }
        return false;
    }
};

struct vertex {
    int i, j;

    bool operator==(const vertex& other) const {
        return i == other.i && j == other.j;
    }

    bool operator!=(const vertex& other) const {
        return i != other.i || j != other.j;
    }

    bool operator<(const vertex& other) const {
        if (i < other.i) {
            return true;
        } else if (i == other.i) {
            return j < other.j;
        }
        return false;
    }
};

namespace std {
    template<>
    struct hash<vertex> {
        size_t operator()(const vertex &x) const {
            size_t seed = 0;
            seed ^= std::hash<int>()(x.i) + 0x9e3779b9
                                + (seed << 6) + (seed >> 2);
            seed ^= std::hash<int>()(x.i) + 0x9e3779b9
                                  + (seed << 6) + (seed >> 2);

            return seed;
        }
    };
}

struct croppedNode{
    double F, g, H;
    vertex parent;

    void calc_F() {
        F = g + H;
    }
};

struct extNode : croppedNode {
    vertex coord;
};

#endif
