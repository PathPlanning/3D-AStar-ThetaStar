#ifndef ISEARCH_H
#define ISEARCH_H

#include "list.h"
#include "map.h"
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include "node.h"
#include "minqueue.h"

#include <unordered_map>
#include <math.h>

class ISearch {
public:
    ISearch();

    //virtual ~ISearch(void);

    SearchResult startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options);

protected:
    SearchResult sresult;
    int sizelimit; //����������� �� ������ OPEN
    double hweight; //��� ���������
    int breakingties; //�������� ������ ��������� ������� �� OPEN, ����� F-�������� �����
    bool is_any_angle_search = false;

    virtual double heuristic(int i, int j, int goal_i, int goal_j, const EnvironmentOptions &options) const;

    virtual void add_successors_to_opened(const extNode &pos, minqueue &opened,
                                          const std::map<vertex, std::pair<double, vertex>> &closed,
                                          const EnvironmentOptions &options, const Map &map) = 0;

    bool is_diagonal_move(int direction) const;

    bool is_straight_move(int direction) const;

    vertex make_one_step(const vertex &position, int direction) const;

    /*
     * Do not return correct g, f, h, parent fields of node
     */
    Node make_one_step(const Node &position, int direction) const;

    /*
     * Returns direction between point according to gl_const
     */
    int direction(const vertex &from, const vertex &to) const;

    int direction(int delta_i, int delta_j) const;

    int direction(const Node &from, const Node &to) const;

    /*
     * Distance between points in cells for diagonal and straight moves
     */
    int distance(const vertex &from, const vertex &to) const;

    void add_segment_to_path(const Node begin, const Node end, NodeList &path, const EnvironmentOptions &options) const;

    double euclid_distance(const vertex &from, const vertex &to, double linecost) const;

    double euclid_distance(const Node &from, const Node &to, double linecost) const;

    bool line_of_sight(const vertex &from, const vertex &to, const Map &map) const;

    void construct_line(const Node &from, const Node &to, NodeList& out, const EnvironmentOptions &options) const;
};

#endif
