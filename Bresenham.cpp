#include "Bresenham.h"
#include "node.h"
#include <algorithm>
#include <stdexcept>

void iBresenham::bresenham3d(int x1, int y1, int z1, const int x2, const int y2, const int z2) {

    int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
    int point[3];

    point[0] = x1;
    point[1] = y1;
    point[2] = z1;
    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    if (dx == 0) {
        x_inc = 0;
    } else if (dx < 0) {
        x_inc = -1;
    } else {
        x_inc = 1;
    }
    l = abs(dx);
    if (dy == 0) {
        y_inc = 0;
    } else if (dy < 0) {
        y_inc = -1;
    } else {
        y_inc = 1;
    }
    m = abs(dy);
    if (dz == 0) {
        z_inc = 0;
    } else if (dz < 0) {
        z_inc = -1;
    } else {
        z_inc = 1;
    }
    n = abs(dz);
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;

    if ((l >= m) && (l >= n)) {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i < l; i++) {
            if (!ProcessPoint(point[0], point[1], point[2])) {
                return;
            }
            if (err_1 > 0) {
                point[1] += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) {
                point[2] += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            point[0] += x_inc;
        }
    } else if ((m >= l) && (m >= n)) {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i < m; i++) {
            if (!ProcessPoint(point[0], point[1], point[2])) {
                return;
            }
            if (err_1 > 0) {
                point[0] += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) {
                point[2] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            point[1] += y_inc;
        }
    } else {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i < n; i++) {
            if (!ProcessPoint(point[0], point[1], point[2])) {
                return;
            }
            if (err_1 > 0) {
                point[1] += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) {
                point[0] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            point[2] += z_inc;
        }
    }
    if (!ProcessPoint(point[0], point[1], point[2])) {
        return;
    }
}

LineOfSight::LineOfSight(const Map& init_map) : force_stopped(false), map(init_map) {}

bool LineOfSight::ProcessPoint(int i, int j, int h) {
    if (map.CellIsObstacle(i, j, h)) {
        force_stopped = true;
        return false;
    }
    return true;
}

bool LineOfSight::line_of_sight(int i0, int j0, int h0, int i1, int j1, int h1) {
    bresenham3d(i0, j0, h0, i1, j1, h1);
    return !force_stopped;
}

bool LineOfSight::line_of_sight(const Node &from, const Node &to) {
    return line_of_sight(from.i, from.j, from.z, to.i, to.j, to.z);
}

Liner::Liner(const Map &init_map, std::list<Node> *init_path) : map(init_map), path(init_path) {}

bool Liner::ProcessPoint(int i, int j, int h) {
    Node newNode;
    newNode.i = i;
    newNode.j = j;
    newNode.z = h;
    path->push_back(newNode);
    return true;
}

void Liner::append_line(int i0, int j0, int h0, int i1, int j1, int h1) {
    bresenham3d(i0, j0, h0, i1, j1, h1);
}

void Liner::append_line(const Node &from, const Node &to) {
    bresenham3d(from.i, from.j, from.z, to.i, to.j, to.z);
}
