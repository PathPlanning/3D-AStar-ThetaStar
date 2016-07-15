#include "gl_const.h"
#include "minqueue.h"
#include "isearch.h"
#include "searchresult.h"
#include "node.h"
#include "list.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <map>
#include <queue>
#include <unordered_map>


ISearch::ISearch() {
}

ISearch::~ISearch(void) {
}

SearchResult ISearch::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::map<vertex, std::pair<double, vertex>> closed;
    //auto comp = [](const extNode &x, const extNode &y) { return x.F > y.F; };
    //std::priority_queue<extNode, std::vector<extNode>, decltype(comp)> opened(comp);
    minqueue opened(breakingties);

    extNode current;
    current.coord = {map.start_i, map.start_j};
    current.g = 0;
    current.H = heuristic(map.start_i, map.start_j, map.goal_i, map.goal_j, options);
    current.calc_F();
    opened.push(current);

    int i, j;
    bool is_diag_move;
    extNode upd;
    do {
        current = opened.top();
        opened.pop();
        i = current.coord.i;
        j = current.coord.j;
        if (closed.find({i, j}) == closed.end()) {
            closed[{i, j}] = {current.g, current.parent};
            for (int add_i = -1; add_i != 2; ++add_i) {
                for (int add_j = -1; add_j != 2; ++add_j) {
                    // Skip the same point
                    if (add_i == 0 && add_j == 0) {
                        continue;
                    }

                    is_diag_move = add_i != 0 && add_j != 0;
                    // Skip diagonal move, if not allowed
                    if (is_diag_move && !options.allowdiagonal) {
                        continue;
                    }

                    // TODO check squeeze

                    if (map.CellOnGrid(i + add_i, j + add_j)
                        && map.CellIsTraversable(i + add_i, j + add_j)
                        && closed.find({i + add_i, j + add_j}) == closed.end()) {
                        upd.coord = {i + add_i, j + add_j};
                        upd.g = current.g + (is_diag_move ? options.diagonalcost : options.linecost);
                        upd.H = heuristic(i + add_i, j + add_j, map.goal_i, map.goal_j, options);
                        upd.parent = current.coord;
                        upd.calc_F();
                        opened.push(upd);
                    }
                }
            }
        }
    } while (!(current.coord.i == map.goal_i && current.coord.j == map.goal_j) && !opened.empty());

    if (current.coord.i == map.goal_i && current.coord.j == map.goal_j) {
        sresult.pathfound = true;
        sresult.pathlength = current.g;
        auto path = new NodeList;
        vertex point = current.coord;
        auto path_cost = current.g;
        do {
            path->List.push_front({
                                          point.i,
                                          point.j,
                                          0,
                                          path_cost,
                                          0,
                                          nullptr
                                  });
            point = closed[point].second;
            path_cost = closed[point].first;
        } while (!(point.i == map.start_i && point.j == map.start_j));

        path->List.push_front({
                                      point.i,
                                      point.j,
                                      0,
                                      path_cost,
                                      0,
                                      nullptr
                              });
        sresult.lppath = path;
        // TODO correct hppath count
        sresult.hppath = path;
    } else {
        sresult.pathfound = false;
        sresult.pathlength = 0;
    }

    auto finish_time = std::chrono::high_resolution_clock::now();
    sresult.time = std::chrono::duration_cast<std::chrono::microseconds>(finish_time - start_time).count();
    sresult.time /= 1000000;
    sresult.numberofsteps = closed.size();
    sresult.nodescreated = closed.size() + opened.size();

    auto foramated_viewed = new std::map<Node, bool>;
    // TODO add opened list output

    for (auto it = closed.begin(); it != closed.end(); ++it) {
        i = it->first.i;
        j = it->first.j;
        double g = it->second.first;
        double h = heuristic(i, j, map.goal_i, map.goal_j, options);
        double f = g + h;
        foramated_viewed->operator[]({
                i,
                j,
                f,
                g,
                h,
                nullptr
                                 }) = true;
    }

    sresult.viewed = foramated_viewed;

    return sresult;
}

