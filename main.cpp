#include <iostream>
#include <cstdint>
#include <functional>
#include <fstream>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <sstream>
#include <vector>
#include <queue>

#include "TinyXML2/tinyxml2.h"

struct SearchTask {
    size_t width, height;
    long double line_cost, diagonal_cost, heuristic_weight;
    std::pair<uint_fast32_t, uint_fast32_t> start, finish;
    bool allow_diagonal_move;
    std::vector<std::vector<bool>> field_map;
};

class GraphSearcher {
public:
    virtual long double heuristic(uint_fast32_t x, uint_fast32_t y) const = 0;

    typedef std::pair<uint_fast32_t, uint_fast32_t> vertex;

    long double heuristic(vertex v) const {
        return heuristic(v.first, v.second);
    }

private:
    size_t max_y, max_x;
    std::vector<std::vector<bool>> graph_matrix;
    long double straight_cost, diag_cost;
    bool diag_allowed;

    struct queue_node {
        vertex v;
        vertex parent;
        long double g_value;
        long double h_value;

        long double f_val() const {
            return g_value + h_value;
        }

        // !!! It is a GREATER operator. Inversion is made for STL priority queue
        bool operator<(const queue_node &other) const {
            return f_val() > other.f_val();
        }
    };

    class priority_queue {
    private:
        std::map<vertex, size_t> indexes;
        std::vector<queue_node> data;
        size_t last_not_fake;

        void sift_up(size_t pos) {
            queue_node tmp;
            while (pos > 1 && data[pos].f_val() < data[pos >> 1].f_val()) {
                indexes[data[pos].v] = pos >> 1;
                indexes[data[pos >> 1].v] = pos;
                tmp = data[pos];
                data[pos] = data[pos >> 1];
                data[pos >> 1] = tmp;
                pos >>= 1;
            }
        }

        void sift_down(size_t pos) {
            size_t to_swap;
            queue_node tmp;
            while (2 * pos <= last_not_fake) {
                to_swap = pos;
                if (data[2 * pos].f_val() < data[to_swap].f_val()) {
                    to_swap = 2 * pos;
                }
                if (2 * pos + 1 <= last_not_fake &&
                    data[2 * pos + 1].f_val() < data[to_swap].f_val()) {
                    to_swap = 2 * pos + 1;
                }
                if (to_swap == pos) {
                    return;
                }

                indexes[data[pos].v] = to_swap;
                indexes[data[to_swap].v] = pos;
                tmp = data[pos];
                data[pos] = data[to_swap];
                data[to_swap] = tmp;
                pos = to_swap;
            }
        }

    public:
        priority_queue() : indexes(), data(1), last_not_fake(0) { }

        bool exist(vertex v) const {
            return indexes.find(v) != indexes.end();
        }

        void insert(queue_node val) {
            ++last_not_fake;
            if (last_not_fake == data.size()) {
                data.push_back(queue_node());
            }
            data[last_not_fake] = val;
            indexes[val.v] = last_not_fake;
            sift_up(last_not_fake);
        }

        void update(vertex v, long double new_path, long double new_h) {
            size_t id = indexes[v];
            queue_node node = data[id];
            if (new_path < node.g_value) {
                data[id].g_value = new_path;
                data[id].h_value = new_h;
                sift_up(id);
            }
        }

        queue_node top() {
            return data[1];
        }

        bool empty() {
            return !last_not_fake;
        };

        void pop() {
            indexes.erase(data[1].v);
            data[1] = data[last_not_fake];
            indexes[data[1].v] = 1;
            --last_not_fake;
            sift_down(1);
        }
    };

    /*
     * Generates correct adj(v)
     */
    std::vector<vertex> neigbors(vertex v) const {
        std::vector<vertex> all;
        all.reserve(8);

        if (v.first) {
            all.push_back({v.first - 1, v.second});

            if (diag_allowed) {
                if (v.second) {
                    all.push_back({v.first - 1, v.second - 1});
                }
                if (v.second + 1 < max_y) {
                    all.push_back({v.first - 1, v.second + 1});
                }
            }
        }
        if (v.first + 1 < max_x) {
            all.push_back({v.first + 1, v.second});

            if (diag_allowed) {
                if (v.second) {
                    all.push_back({v.first + 1, v.second - 1});
                }
                if (v.second + 1 < max_y) {
                    all.push_back({v.first + 1, v.second + 1});
                }
            }
        }
        if (v.second) {
            all.push_back({v.first, v.second - 1});
        }
        if (v.second + 1 < max_y) {
            all.push_back({v.first, v.second + 1});
        }

        std::vector<vertex> res;
        res.reserve(8);
        for (size_t i = 0; i < all.size(); ++i) {
            if (!graph_matrix[all[i].first][all[i].second]) {
                res.push_back(all[i]);
            }
        }

        return res;
    }

    long double get_cost(vertex from, vertex to) const {
        if (from == to) {
            return 0;
        }
        if (from.first == to.first || from.second == to.second) {
            return straight_cost;
        }

        return diag_cost;
    }

public:

    void load_graph(size_t width,
                    size_t height,
                    const std::vector<std::vector<bool>> &matrix_map,
                    long double line_cost,
                    bool diagonal_move_allowed = false,
                    long double diagonal_cost = 0.0) {
        max_x = height;
        max_y = width;
        straight_cost = line_cost;
        diag_allowed = diagonal_move_allowed;
        diag_cost = diagonal_cost;
        graph_matrix = matrix_map;
    }

    GraphSearcher(const SearchTask &task) {
        load_graph(task.width, task.height, task.field_map, task.line_cost, task.allow_diagonal_move,
                   task.diagonal_cost);
    }

    long double graph_search(vertex start, vertex finish,
                             bool recover_path = false, std::list<vertex> *path = nullptr,
                             bool record_visited = false,
                             std::map<vertex, bool> *visited = nullptr) const {
        std::priority_queue<queue_node> opened;
        std::map<vertex, vertex> closed;
        std::map<vertex, long double> g_val;
        opened.push({start, start, 0, heuristic(start)});
        g_val[start] = 0;

        queue_node current;
        long double upd_cost;
        do {
            current = opened.top();
            opened.pop();

            if (closed.find(current.v) == closed.end()) {
                for (vertex u : neigbors(current.v)) {
                    if (g_val.find(u) == g_val.end()) {
                        g_val[u] = std::numeric_limits<long double>::infinity();

                        if (record_visited){
                            visited->operator[](current.v) = false;
                        }
                    }
                    upd_cost = g_val[current.v] + get_cost(current.v, u);
                    if (upd_cost < g_val[u]) {
                        g_val[u] = upd_cost;
                        opened.push({u, current.v, upd_cost, heuristic(u)});
                    }
                }

                closed[current.v] = current.parent;
                if (record_visited){
                    visited->operator[](current.v) = true;
                }
            }
        } while (current.v != finish && !opened.empty());

        if (recover_path && closed.find(finish) != closed.end()) {
            vertex point = finish;
            do {
                path->push_front(point);
                point = closed[point];
            } while (point != start);
        }

        if (current.v == finish) {
            return current.g_value;
        } else {
            return std::numeric_limits<long double>::infinity();
        }
    }
};

class DijkstraSearcher : public GraphSearcher {
public:
    long double heuristic(uint_fast32_t, uint_fast32_t) const {
        return 0;
    }

    DijkstraSearcher(const SearchTask &task) : GraphSearcher(task) { }
};

class Astar : public GraphSearcher {
private:
    const uint_fast32_t C_d = 1;
    const uint_fast32_t C_hv = 1;
    long double weight;
    GraphSearcher::vertex finish;
public:
    long double delta_heuristic(uint_fast32_t x, uint_fast32_t y) const {
        uint_fast32_t delta_x = (x > finish.first ? (x - finish.first) : (finish.first - x));
        uint_fast32_t delta_y = (y > finish.second ? (y - finish.second) : (finish.second - y));

        if (delta_x > delta_y) {
            return C_d * delta_y + C_hv * (delta_x - delta_y);
        } else {
            return C_d * delta_x + C_hv * (delta_y - delta_x);
        }
    }

    long double euclid_heuristic(uint_fast32_t x, uint_fast32_t y) const {
        uint_fast32_t delta_x = (x > finish.first ? (x - finish.first) : (finish.first - x));
        uint_fast32_t delta_y = (y > finish.second ? (y - finish.second) : (finish.second - y));
        return std::sqrt(std::pow(delta_x, 2) + std::pow(delta_y, 2));
    }

    long double heuristic(uint_fast32_t x, uint_fast32_t y) const {
        return weight * euclid_heuristic(x, y);
    }

    Astar(const SearchTask &task) : GraphSearcher(task) {
        finish = task.finish;
        weight = task.heuristic_weight;
    }
};

SearchTask parse(const char *filename) {
    SearchTask parsed_data;

    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename);

    auto algo_options = doc.RootElement()->FirstChildElement("algorithm");
    parsed_data.allow_diagonal_move = std::stoi(algo_options->FirstChildElement("allowdiagonal")->GetText());
    if (parsed_data.allow_diagonal_move) {
        parsed_data.diagonal_cost = std::stold(algo_options->FirstChildElement("diagonalcost")->GetText());
    } else {
        parsed_data.diagonal_cost = std::numeric_limits<long double>::infinity();
    }
    parsed_data.line_cost = std::stold(algo_options->FirstChildElement("linecost")->GetText());
    parsed_data.heuristic_weight = std::stold(algo_options->FirstChildElement("hweight")->GetText());


    auto map_tag = doc.RootElement()->FirstChildElement("map");

    parsed_data.width = std::stoul(map_tag->FirstChildElement("width")->GetText());
    parsed_data.height = std::stoul(map_tag->FirstChildElement("height")->GetText());

    parsed_data.start.first = std::stoul(map_tag->FirstChildElement("startx")->GetText());
    parsed_data.start.second = std::stoul(map_tag->FirstChildElement("starty")->GetText());

    parsed_data.finish.first = std::stoul(map_tag->FirstChildElement("finishx")->GetText());
    parsed_data.finish.second = std::stoul(map_tag->FirstChildElement("finishy")->GetText());

    auto grid = map_tag->FirstChildElement("grid");
    parsed_data.field_map = std::vector<std::vector<bool>>(parsed_data.height);
    std::vector<bool> row(parsed_data.width);
    tinyxml2::XMLElement *node = grid->FirstChildElement("row");
    int tmp;

    for (size_t i = 0; i < parsed_data.height; ++i) {
        std::stringstream istr(node->GetText(), std::ios_base::in);
        for (size_t j = 0; j < parsed_data.width; ++j) {
            istr >> tmp;
            row[j] = tmp == 1;
        }
        parsed_data.field_map[i] = row;
        node = node->NextSiblingElement("row");
    }

    return parsed_data;
}

void evaluate_task(const char *task_file_path, const char *output_file_path) {
    SearchTask test_task = parse(task_file_path);

    Astar Searcher(test_task);

    std::list<GraphSearcher::vertex> path;
    std::map<GraphSearcher::vertex, bool> visited_list;

    std::cout << "Path found with cost: \t"
    << Searcher.graph_search(test_task.start, test_task.finish, true, &path, true, &visited_list) << "\n\n";

    std::ofstream out(output_file_path);
    out << test_task.height << '\n' << test_task.width << "\n\n";
    for (size_t i = 0; i < test_task.height; ++i) {
        for (size_t j = 0; j < test_task.width; ++j) {
            out << test_task.field_map[i][j] << ' ';
        }
        out << '\n';
    }
    out << '\n';

    for (auto node : path) {
        out << node.first << ' ' << node.second << '\n';
    }
    out << '\n';

    for (auto It = visited_list.begin(); It != visited_list.end(); ++It) {
        out << It->first.first << ' ' << It->first.second << ' ' << It->second << '\n';
    }

    out.close();
}

int main() {
    std::string dir_path;
    std::cout << "Enter path to the task directory (must contain task_list.txt file):  ";
    std::cin >> dir_path;

    std::ifstream list_f(std::string(dir_path).append("/task_list.txt"));

    std::string filename;
    std::string full_path;
    std::string out_path;
    while (list_f) {
        list_f >> filename;
        std::cout << "Processing " << filename << "...\n";
        full_path = std::string(dir_path) + "/" + filename;
        out_path = std::string(dir_path) + "/" + filename + ".path.txt";
        evaluate_task(full_path.c_str(), out_path.c_str());
    }


    return 0;
}