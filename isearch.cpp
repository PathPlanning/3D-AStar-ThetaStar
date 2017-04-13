#include "gl_const.h"
#include "isearch.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <chrono>

ISearch::ISearch() {
    //��������� ������ ���� ��������� �������������� ������������, � ����������� �� ����, ����� ������������ "������������" ���������
    hweight = 1;
    breakingties = CN_SP_BT_GMAX;
    openSize = 0;
    //sresult ���������� �� ���� - � ���� ���� ����������� �� ���������...
}

ISearch::~ISearch(void) {}

double ISearch::MoveCost(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h,
                         const EnvironmentOptions &options) {
    //Assuming that we work in a Euclidean space
    int diff = abs(start_i - fin_i) + abs(start_j - fin_j) + abs(start_h - fin_h);
    switch (diff) {
        case 1:
            return options.linecost;
        case 2:
            return M_SQRT2 * options.linecost;
        case 3:
            return sqrt(3) * options.linecost;
        default:
        case 0:
            return 0;
    }
}

bool ISearch::stopCriterion() {
    if (openSize == 0) {
        std::cout << "OPEN list is empty!" << std::endl;
        return true;
    }
    return false;
}

SearchResult ISearch::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    open = std::list<Node>();

    Node curNode;
    curNode.i = map.start_i;
    curNode.j = map.start_j;
    curNode.z = map.start_h;
    curNode.g = 0;
    curNode.H = computeHFromCellToCell(curNode.i, curNode.j, curNode.z, map.goal_i, map.goal_j, map.goal_h, options);
    curNode.F = hweight * curNode.H;
    curNode.parent = nullptr;

    addOpen(curNode, curNode.get_id(map.height, map.width));
    bool pathfound = false;
    const Node *curIt;
    size_t closeSize = 0;
    openSize = 1;
    while (!stopCriterion()) {
        curNode = findMin(map.height);
        close.insert({curNode.i * map.width + curNode.j + map.height * map.width * curNode.z, curNode});
        ++closeSize;
        deleteMin(curNode, curNode.get_id(map.height, map.width));
        --openSize;
        if (curNode.i == map.goal_i && curNode.j == map.goal_j) {
            pathfound = true;
            break;
        }
        std::list<Node> successors = findSuccessors(curNode, map, options);
        auto it = successors.begin();
        auto parent = &(close.find(curNode.i * map.width + curNode.j + map.height * map.width * curNode.z)->second);
        while (it != successors.end()) {
            it->parent = parent;
            it->H = computeHFromCellToCell(it->i, it->j, it->z, map.goal_i, map.goal_j, map.goal_h, options);
            *it = resetParent(*it, *it->parent, map, options);
            it->F = it->g + hweight * it->H;
            addOpen(*it, it->get_id(map.height, map.width));
            it++;
        }
    }
    sresult.pathfound = false;
    sresult.nodescreated = closeSize + openSize;
    sresult.numberofsteps = closeSize;
    //Logger->writeToLogOpenClose(open, close, map.height, true); //������ ��� ��������� ������ ��� � ��� ��� ���.
    if (pathfound) {
        sresult.pathfound = true;
        //���� ��������������� �� �������� ���������� (��� ���� ����������)
        makePrimaryPath(curNode);
        sresult.hppath = &hppath;
        sresult.pathlength = curNode.g;
    }
    //�.�. �������������� ���� �� �������� ���������� - ������������ ����� ����������, ����� ������������� ������ ������!
    end = std::chrono::system_clock::now();
    sresult.time =
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / 1000000;
    //������������� ���� (hplevel, ���� lplevel, � ����������� �� ���������)
    if (pathfound) makeSecondaryPath(map, curNode);
    return sresult;
}

std::list<Node> ISearch::findSuccessors(Node curNode, const Map &map, const EnvironmentOptions &options) {
    Node newNode;
    std::list<Node> output;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            for (int h = -1; h <= 1; ++h) {
                if ((i != 0 || j != 0 || h != 0) && map.CellOnGrid(curNode.i + i, curNode.j + j, curNode.z + h) &&
                    (map.CellIsTraversable(curNode.i + i, curNode.j + j, curNode.z + h))) {
                    if (options.allowdiagonal == CN_SP_AD_FALSE && abs(i) + abs(j) + abs(h) > 1) {
                        continue;
                    }
                    if (options.allowsqueeze == CN_SP_AS_FALSE && (i != 0 && j != 0)) {
                        int min_obs_height = std::min(map.getValue(curNode.i, curNode.j + j),
                                                      map.getValue(curNode.i + i, curNode.j));
                        if (min_obs_height > std::max(curNode.z, curNode.z + h)) {
                            continue;
                        }
                    }
                    if (options.allowcutcorners == CN_SP_AC_FALSE &&
                        (map.CellIsObstacle(curNode.i + i, curNode.j + j, curNode.z) ||
                         map.CellIsObstacle(curNode.i + i, curNode.j, curNode.z + h) ||
                         map.CellIsObstacle(curNode.i, curNode.j + j, curNode.z + h))) {
                        continue;
                    }
                    newNode.i = curNode.i + i;
                    newNode.j = curNode.j + j;
                    newNode.z = curNode.z + h;
                    if (close.find(newNode.i * map.width + newNode.j + map.height * map.width * newNode.z) ==
                        close.end()) {
                        newNode.g = curNode.g + MoveCost(curNode.i, curNode.j, curNode.z, curNode.i + i, curNode.j + j,
                                                         curNode.z + h, options);
                        output.push_back(newNode);
                    }
                }
            }
        }
    }
    return output;
}

void ISearch::makePrimaryPath(Node curNode) {
    Node current = curNode;
    while (current.parent != nullptr) {
        lppath.List.push_front(current);
        current = *current.parent;
    }
    lppath.List.push_front(current);
    sresult.lppath = &lppath; //����� � sresult - ��������� �� ���������.
}

void ISearch::makeSecondaryPath(const Map &map, Node curNode) {
    std::list<Node>::const_iterator iter = lppath.List.begin();
    Node cur, next, move;
    hppath.List.push_back(*iter);

    while (iter != --lppath.List.end()) {
        cur = *iter;
        ++iter;
        next = *iter;
        move.i = next.i - cur.i;
        move.j = next.j - cur.j;
        move.z = next.z - cur.z;
        ++iter;
        if ((iter->i - next.i) != move.i || (iter->j - next.j) != move.j || (iter->z - next.z) != move.z)
            hppath.List.push_back(*(--iter));
        else
            --iter;
    }
    sresult.hppath = &hppath;
}

Node ISearch::findMin(int size) {
    return *open.begin();
}

void ISearch::deleteMin(Node minNode, uint_least32_t key) {
    open.pop_front();
}

void ISearch::addOpen(Node newNode, uint_least32_t key) {
    std::list<Node>::iterator position = open.end();
    std::list<Node>::iterator found = open.end();
    for (auto it = open.begin(); it != open.end() && (position == open.end() || found == open.end()); ++it) {
        if (position == open.end()) {
            if (newNode.F < it->F) {
                position = it;
            } else if (newNode.F == it->F) {
                if (breakingties == CN_SP_BT_GMAX && newNode.g > it->g ||
                    breakingties == CN_SP_BT_GMIN && newNode.g < it->g)
                    position = it;
            }
        }

        if (newNode.i == it->i && newNode.j == it->j && newNode.z == it->z) {
            found = it;
        }
    }
    if (found != open.end()) {
        if (newNode.g < found->g) {
            if (found == position) {
                *found = newNode;
            } else {
                open.erase(found);
                open.insert(position, newNode);
            }
        }
    } else {
        ++openSize;
        open.insert(position, newNode);
    }
}