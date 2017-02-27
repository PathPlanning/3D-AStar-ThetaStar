#ifndef NODE_H
#define NODE_H

#include <functional>
struct Node
{
    int i, j, z;
    double F, g, H;
    const Node* parent;

    bool operator==(const Node& other) const;
    bool operator!=(const Node& other) const;
};

namespace std {
    template<>
    struct hash<Node> {
        size_t operator()(const Node &x) const;
    };
}
#endif
