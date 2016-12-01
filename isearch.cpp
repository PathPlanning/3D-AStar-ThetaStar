#include "gl_const.h"
#include "isearch.h"


ISearch::ISearch() {
    //��������� ������ ���� ��������� �������������� ������������, � ����������� �� ����, ����� ������������ "������������" ���������
    hweight = 1;
    breakingties = CN_SP_BT_GMAX;
    sizelimit = CN_SP_SL_NOLIMIT;
    open = NULL;
    openSize = 0;
    //sresult ���������� �� ���� - � ���� ���� ����������� �� ���������...
}

ISearch::~ISearch(void) {
    if (open) {
        delete open;
    }
}

double ISearch::MoveCost(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h,
                         const EnvironmentOptions &options) {
    //Assuming that we work in a Euclidean space
    return options.linecost * sqrt(abs(start_i - fin_i) + abs(start_j - fin_j) + abs(start_h - fin_h));
}

bool ISearch::stopCriterion() {
    if (open->empty()) {
        std::cout << "OPEN list is empty!" << std::endl;
        return true;
    }
    return false;
}

SearchResult ISearch::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    open = new ClusteredSets(map.height, breakingties);

    Node curNode;
    curNode.i = map.start_i;
    curNode.j = map.start_j;
    curNode.z = map.start_h;
    curNode.g = 0;
    curNode.H = computeHFromCellToCell(curNode.i, curNode.j, curNode.z, map.goal_i, map.goal_j, map.goal_h, options);
    curNode.F = hweight * curNode.H;
    curNode.parent = nullptr;

    addOpen(curNode);
    int closeSize = 0;
    bool pathfound = false;
    const Node *curIt;
    std::vector<Node> successors;
    successors.reserve(26); // Usually every node has no more than 26 successors
    while (!stopCriterion()) {
        curNode = open->FindMin();
        curIt = &(*(close.insert(curNode).first));
        ++closeSize;
        open->DeleteMin();
        --openSize;

        if (curNode.i == map.goal_i && curNode.j == map.goal_j && curNode.z == map.goal_h) {
            pathfound = true;
            break;
        }

        successors.resize(0);
        findSuccessors(curNode, map, options, successors);
        for (auto it = successors.begin(); it != successors.end(); ++it) {
            it->parent = curIt;
            it->H = computeHFromCellToCell(it->i, it->j, it->z, map.goal_i, map.goal_j, map.goal_h, options);
            *it = resetParent(*it, *it->parent, map, options);
            it->F = it->g + hweight * it->H;
            addOpen(*it);
        }

        //Logger->writeToLogOpenClose(open, close, map.height); //������ ��� ��������� ������ ��� � ��� ��� ���.
    }
    //����� ����������!
    sresult.pathfound = false;
    sresult.nodescreated = close.size() + open->size();
    sresult.numberofsteps = close.size();
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
    if (pathfound)
        makeSecondaryPath(map, curNode);
    return sresult;
}

void ISearch::findSuccessors(Node curNode, const Map &map, const EnvironmentOptions &options, std::vector<Node> &output) {
    Node newNode;
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
                    newNode.i = curNode.i + i;
                    newNode.j = curNode.j + j;
                    newNode.z = curNode.z + h;
                    if (close.find(newNode) == close.end()) {
                        newNode.g = curNode.g + MoveCost(curNode.i, curNode.j, curNode.z, curNode.i + i, curNode.j + j,
                                                         curNode.z + h, options);
                        output.push_back(newNode);
                    }
                }
            }
        }
    }
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
        iter++;
        next = *iter;
        move.i = next.i - cur.i;
        move.j = next.j - cur.j;
        move.z = next.z - cur.z;
        iter++;
        if ((iter->i - next.i) != move.i || (iter->j - next.j) != move.j || (iter->z - next.z) != move.z)
            hppath.List.push_back(*(--iter));
        else
            iter--;
    }
    sresult.hppath = &hppath;
}
