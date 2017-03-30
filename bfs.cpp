#include "bfs.h"

BFS::BFS() {
    hweight = 1;
}

BFS::~BFS()
{
}

void BFS::addOpen(Node newNode, uint_least32_t key) {
    newNode.F = newNode.H;
    ISearch::addOpen(newNode, key);
}