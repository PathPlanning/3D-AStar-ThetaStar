#include "node.h"


bool Node::operator==(const Node &other) const {
    return i == other.i && j == other.j && z == other.z;
}

bool Node::operator!=(const Node &other) const {
    return !operator==(other);
}

uint_least32_t Node::get_id(int map_height, int map_width) {
    return (uint_least32_t)map_height * map_width * z + map_width * i + j;
}

size_t std::hash<Node>::operator()(const Node &x) const {
    size_t seed = 0;
    seed ^= std::hash<int>()(x.i) + 0x9e3779b9
            + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>()(x.j) + 0x9e3779b9
            + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>()(x.z) + 0x9e3779b9
            + (seed << 6) + (seed >> 2);

    return seed;
}
