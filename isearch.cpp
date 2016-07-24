#include "gl_const.h"
#include "minqueue.h"
#include "isearch.h"

#include <vector>
#include <chrono>
#include <queue>


ISearch::ISearch() {
}

int ISearch::direction(const vertex &from, const vertex &to) const {
    int delta_i = to.i - from.i;
    int delta_j = to.j - from.j;
    return direction(delta_i, delta_j);
}

int ISearch::direction(int delta_i, int delta_j) const {
    if (delta_i > 0) {
        if (delta_j > 0) {
            return CN_MD_DOWN_RIGHT;
        }
        if (delta_j == 0) {
            return CN_MD_DOWN;
        }
        return CN_MD_DOWN_LEFT;
    }
    if (delta_i == 0) {
        if (delta_j > 0) {
            return CN_MD_RIGHT;
        }
        if (delta_j == 0) {
            return CN_MD_STATIC;
        }
        return CN_MD_LEFT;
    }
    if (delta_j > 0) {
        return CN_MD_UP_RIGHT;
    }
    if (delta_j == 0) {
        return CN_MD_UP;
    }
    return CN_MD_UP_LEFT;
}

int ISearch::direction(const Node &from, const Node &to) const {
    int delta_i = to.i - from.i;
    int delta_j = to.j - from.j;
    return direction(delta_i, delta_j);
}

int ISearch::distance(const vertex &from, const vertex &to) const {
    // Supposed that two points lies on diagonal or straight line
    return std::max(std::abs(to.i - from.i), std::abs(to.j - from.j));
}

vertex ISearch::make_one_step(const vertex &position, int direction) const {
    switch (direction) {
        case CN_MD_STATIC:
            return position;
        case CN_MD_UP:
            return {position.i - 1, position.j};
        case CN_MD_UP_RIGHT:
            return {position.i - 1, position.j + 1};
        case CN_MD_RIGHT:
            return {position.i, position.j + 1};
        case CN_MD_DOWN_RIGHT:
            return {position.i + 1, position.j + 1};
        case CN_MD_DOWN:
            return {position.i + 1, position.j};
        case CN_MD_DOWN_LEFT:
            return {position.i + 1, position.j - 1};
        case CN_MD_LEFT:
            return {position.i, position.j - 1};
        case CN_MD_UP_LEFT:
            return {position.i - 1, position.j - 1};
    }
}

Node ISearch::make_one_step(const Node &position, int direction) const {
    Node result = position;
    switch (direction) {
        case CN_MD_UP:
            --result.i;
            break;
        case CN_MD_UP_RIGHT:
            --result.i;
            ++result.j;
            break;
        case CN_MD_RIGHT:
            ++result.j;
            break;
        case CN_MD_DOWN_RIGHT:
            ++result.i;
            ++result.j;
            break;
        case CN_MD_DOWN:
            ++result.i;
            break;
        case CN_MD_DOWN_LEFT:
            ++result.i;
            --result.j;
            break;
        case CN_MD_LEFT:
            --result.j;
            break;
        case CN_MD_UP_LEFT:
            --result.i;
            --result.j;
            break;
    }
    return result;
}

bool ISearch::is_diagonal_move(int direction) const {
    switch (direction) {
        case CN_MD_UP_RIGHT:
        case CN_MD_DOWN_RIGHT:
        case CN_MD_DOWN_LEFT:
        case CN_MD_UP_LEFT:
            return true;
        default:
            return false;
    }
}

bool ISearch::is_straight_move(int direction) const {
    switch (direction) {
        case CN_MD_UP:
        case CN_MD_DOWN:
        case CN_MD_LEFT:
        case CN_MD_RIGHT:
            return true;
        default:
            return false;
    }
}

void ISearch::add_segment_to_path(const Node begin, const Node end, NodeList &path,
                                  const EnvironmentOptions &options) const {
    int segment_direction = direction(begin, end);
    bool is_segment_diagonal = is_diagonal_move(segment_direction);
    Node current = begin;
    current.parent = &(*path.List.rbegin());
    while (!(current.i == end.i && current.j == end.j)) {
        path.List.push_back(current);
        current = make_one_step(current, segment_direction);
        current.g += (is_segment_diagonal ? options.diagonalcost : options.linecost);
        current.parent = &(*path.List.rbegin());
    }
}

SearchResult ISearch::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    auto start_time = std::chrono::high_resolution_clock::now();
    decltype(start_time) finish_time;
    std::map<vertex, std::pair<double, vertex>> closed;
    //auto comp = [](const extNode &x, const extNode &y) { return x.F > y.F; };
    //std::priority_queue<extNode, std::vector<extNode>, decltype(comp)> opened(comp);
    minqueue opened(breakingties);

    extNode current;
    current.coord = {map.start_i, map.start_j};
    current.g = 0;
    current.H = heuristic(map.start_i, map.start_j, map.goal_i, map.goal_j, options);
    current.parent = current.coord;
    current.calc_F();
    opened.push(current);

    int i, j;
    bool is_diag_move;
    extNode upd;
    while (!opened.empty()) {
        current = opened.top();
        opened.pop();
        if (closed.find(current.coord) != closed.end()) {
            continue;
        }
        closed[current.coord] = {current.g, current.parent};
        if (current.coord.i == map.goal_i && current.coord.j == map.goal_j) {
            break;
        }
        add_successors_to_opened(current, opened, options, map);
    }

    if (current.coord.i == map.goal_i && current.coord.j == map.goal_j) {
        sresult.pathfound = true;
        sresult.pathlength = current.g;
        auto path = new NodeList;
        vertex point = current.coord;
        auto path_cost = current.g;
        size_t count = 0;
        decltype(path->List.rbegin()) prev;
        bool more_than_one_added = false;
        while (!(point.i == map.start_i && point.j == map.start_j)) {
            ++count;
            path->List.push_front({
                                          point.i,
                                          point.j,
                                          0,
                                          path_cost,
                                          0,
                                          nullptr
                                  });
            if (more_than_one_added) {
                prev->parent = &(*(path->List.begin()));
                ++prev;
            } else {
                prev = path->List.rbegin();
            }
            point = closed[point].second;
            path_cost = closed[point].first;
            more_than_one_added = true;
        }

        path->List.push_front({
                                      point.i,
                                      point.j,
                                      0,
                                      path_cost,
                                      0,
                                      nullptr
                              });
        prev->parent = &(*(path->List.begin()));
        finish_time = std::chrono::high_resolution_clock::now();
        // Supposed that only horizontal or diagonal moves allowed
        auto point_path = new NodeList;
        Node node;
        for (auto it = path->List.begin(); it != path->List.end(); ++it) {
            node = *it;
            if (it != path->List.begin()) {
                add_segment_to_path(*(node.parent), node, *point_path, options);
            }
        }
        node.parent = &(*(point_path->List.rbegin()));
        point_path->List.push_back(node);
        sresult.hppath = path;
        sresult.lppath = point_path;
    } else {
        sresult.pathfound = false;
        sresult.pathlength = 0;
        finish_time = std::chrono::high_resolution_clock::now();
    }

    sresult.time = std::chrono::duration_cast<std::chrono::microseconds>(finish_time - start_time).count();
    sresult.time /= 1000000;
    sresult.numberofsteps = closed.size();
    sresult.nodescreated = closed.size() + opened.size();

    auto foramated_viewed = new std::map<Node, bool>;
    for (auto it = opened.cbegin(); it != opened.cend(); ++it) {
        current = *it;
        i = current.coord.i;
        j = current.coord.j;
        double g = current.g;
        double h = heuristic(i, j, map.goal_i, map.goal_j, options);
        double f = g + h;
        foramated_viewed->operator[]({
                                             i,
                                             j,
                                             f,
                                             g,
                                             h,
                                             nullptr
                                     }) = false;
    }
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

double ISearch::heuristic(int i, int j, int goal_i, int goal_j, const EnvironmentOptions &options) const {
    double result;
    int delta_i, delta_j;
    switch (options.metrictype) {
        case CN_SP_MT_CHEB:
            result = options.linecost * std::max(std::abs(i - goal_i), std::abs(j - goal_j));
            break;
        case CN_SP_MT_MANH:
            result = options.linecost * (std::abs(i - goal_i) + std::abs(j - goal_j));
            break;
        case CN_SP_MT_DIAG:
            delta_i = std::abs(i - goal_i);
            delta_j = std::abs(j - goal_j);
            if (delta_i < delta_j) {
                result = options.diagonalcost * delta_i + options.linecost * (delta_j - delta_i);
            } else {
                result = options.diagonalcost * delta_j + options.linecost * (delta_i - delta_j);
            }
            break;
        default:
        case CN_SP_MT_EUCL:
            result = options.linecost * std::sqrt(std::pow(i - goal_i, 2) + std::pow(j - goal_j, 2));
    }
    return hweight * result;
}

