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

    uint_least32_t get_id(int map_height, int map_width);
};

namespace std {
    template<>
    struct hash<Node> {
        size_t operator()(const Node &x) const;
    };
}
#endif
