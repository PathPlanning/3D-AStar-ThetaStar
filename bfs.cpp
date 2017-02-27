#include "bfs.h"

BFS::BFS() {
    hweight = 1;
}

BFS::~BFS()
{
}

void BFS::addOpen(Node newNode) {
    newNode.F = newNode.H;
    Astar::addOpen(newNode);
}