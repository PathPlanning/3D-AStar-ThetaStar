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
        vertex pos, parent;
        long double path_cost, h_value;

        long double f_val() const {
            return path_cost + h_value;
        }

        bool operator>(const queue_node &other) const {
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
                indexes[data[pos].pos] = pos >> 1;
                indexes[data[pos >> 1].pos] = pos;
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

                indexes[data[pos].pos] = to_swap;
                indexes[data[to_swap].pos] = pos;
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
            indexes[val.pos] = last_not_fake;
            sift_up(last_not_fake);
        }

        void update(vertex v, long double new_path, long double new_h) {
            size_t id = indexes[v];
            queue_node node = data[id];
            if (new_path < node.path_cost) {
                data[id].path_cost = new_path;
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
            indexes.erase(data[1].pos);
            data[1] = data[last_not_fake];
            indexes[data[1].pos] = 1;
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
        if (from.first != to.first && from.second != to.second) {
            return diag_cost;
        }

        return straight_cost;
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
                             bool recover_path = false, std::list<vertex> *path = nullptr) const {
        std::priority_queue<queue_node, std::vector<queue_node>, std::greater<queue_node>> opened;
        std::map<vertex, std::pair<long double, vertex>> closed;
        std::map<vertex, long double> g_val;
        opened.push({start, start, 0, heuristic(start.first, start.second)});
        g_val[start] = 0;

        queue_node current;
        long double upd_cost;
        do {
            current = opened.top();
            opened.pop();

            if (closed.find(current.pos) == closed.end()) {
                for (vertex u : neigbors(current.pos)) {
                    if (g_val.find(u) == g_val.end()) {
                        g_val[u] = std::numeric_limits<long double>::infinity();
                    }
                    upd_cost = g_val[current.pos] + get_cost(current.pos, u);
                    if (upd_cost < g_val[u]) {
                        g_val[u] = upd_cost;
                        opened.push({u, current.pos, upd_cost, heuristic(u)});
                    }
                }

                closed[current.pos] = {current.path_cost, current.parent};
            }
        } while (current.pos != finish && !opened.empty());

        if (recover_path && closed.find(finish) != closed.end()) {
            vertex point = finish;
            do {
                path->push_front(point);
                point = closed[point].second;
            } while (point != start);
        }

        if (current.pos == finish) {
            return current.path_cost;
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
    const uint_fast32_t C_d = 14;
    const uint_fast32_t C_hv = 10;
    long double weight;
    GraphSearcher::vertex finish;
public:
    long double delta_heuristic(uint_fast32_t x, uint_fast32_t y) const {
        uint_fast32_t delta_x = (x > finish.first ? (x - finish.first) : (finish.first - x));
        uint_fast32_t delta_y = (y > finish.first ? (y - finish.first) : (finish.first - y));

        if (delta_x > delta_y) {
            return delta_y + (delta_x - delta_y);
        } else {
            return delta_x + (delta_y - delta_x);
        }
    }

    long double euclid_heuristic(uint_fast32_t x, uint_fast32_t y) const {
        long double square = std::pow(x - finish.first, 2) + std::pow(y - finish.second, 2);
        return std::floor(std::sqrt(square));
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
    std::cout << "Path found with cost: \t"
    << Searcher.graph_search(test_task.start, test_task.finish, true, &path) << "\n\n";

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