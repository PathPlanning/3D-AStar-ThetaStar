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
    open = std::vector<Node>(1);

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
    return open[1];
}

void ISearch::deleteMin(Node minNode, uint_least32_t key) {
    if (open.size() > 1) {
        open[1] = *open.rbegin();
        open.pop_back();
        sift_down(1);
    }
}

void ISearch::addOpen(Node newNode, uint_least32_t key) {
    bool found = false;
    for (size_t k = 1; k != open.size(); ++k) {
        if (open[k].i == newNode.i && open[k].j == newNode.j && open[k].z == newNode.z) {
            found = true;
            if (newNode.g < open[k].g) {
                open[k] = newNode;
                sift_up(k);
                break;
            }
        }
    }
    if (!found) {
        open.push_back(newNode);
        sift_up(open.size() - 1);
        ++openSize;
    }
}

void ISearch::sift_up(int index) {
    bool changed = true;
    Node tmp;
    while (index > 1 && changed) {
        changed = false;
        if (open[index].F < open[index >> 1].F) {
            changed = true;
        } else if (open[index].F == open[index >> 1].F) {
            if (breakingties == CN_SP_BT_GMAX && open[index].g > open[index >> 1].g ||
                breakingties == CN_SP_BT_GMIN && open[index].g < open[index >> 1].g) {
                changed = true;
            }
        }

        if (changed) {
            tmp = open[index >> 1];
            open[index >> 1] = open[index];
            open[index] = tmp;
            index >>= 1;
        }
    }
}

void ISearch::sift_down(int index) {
    bool changed = true;
    int candidate, left_child, right_child;
    Node tmp;
    while (index << 1 < open.size() && changed) {
        candidate = index;
        left_child = index << 1;
        right_child = left_child + 1;
        if (open[left_child].F < open[index].F) {
            candidate = left_child;
        } else if (open[left_child].F == open[index].F) {
            if (breakingties == CN_SP_BT_GMAX && open[left_child].g > open[index].g ||
                breakingties == CN_SP_BT_GMIN && open[left_child].g < open[index].g) {
                candidate = left_child;
            }
        }
        if (right_child < open.size()) {
            if (open[right_child].F < open[candidate].F) {
                candidate = right_child;
            } else if (open[right_child].F == open[candidate].F) {
                if (breakingties == CN_SP_BT_GMAX && open[right_child].g > open[candidate].g ||
                    breakingties == CN_SP_BT_GMIN && open[right_child].g < open[candidate].g) {
                    candidate = right_child;
                }
            }
        }
        changed = candidate != index;
        if (changed) {
            tmp = open[candidate];
            open[candidate] = open[index];
            open[index] = tmp;
            index = candidate;
        }
    }
}