#ifndef ASEARCH_CUTTED_JPS_H
#define ASEARCH_CUTTED_JPS_H

#include "isearch.h"

#include <utility>
#include <vector>
#include <list>

class JPS : public ISearch {
public:
    JPS() : ISearch(){};

    JPS(double weight, int BT, int SL);

protected:
    std::pair<int, int> corner_directions(int direction) const;

    bool is_diagonal_move(int direction) const;

    bool is_straight_move(int direction) const;

    vertex make_one_step(const vertex &position, int direction) const;

    /*
     * Do not return correct g, f, h, parent fields of node
     */
    Node make_one_step(const Node &position, int direction) const;

    /*
     * Distance between points in cells for diagonal and straight moves
     */
    int distance(const vertex &from, const vertex &to) const;

    virtual bool is_pruned_neigbour(const vertex &x, const vertex &neighbour, int direction, const Map &map) const;

    bool has_forced_neigbour(const vertex &x, int direction, bool allow_squeeze, const Map &map) const;

    virtual std::pair<vertex, bool> jump(vertex position, int direction, const vertex &goal,
                                         const EnvironmentOptions &options, const Map &map) const;

    virtual std::list<Node> find_successors(Node *current, const Map &map, const EnvironmentOptions &options) const;

    virtual void recovery_primary_path(Node *finish, int start_i, int start_j);

    virtual void recovery_secondary_path(Node *finish, int start_i, int start_j, const EnvironmentOptions &options);

    void add_segment_to_path(const Node begin, const Node end, NodeList &path, const EnvironmentOptions &options) const;

};

class orthogonalJPS : public JPS {
public:
    orthogonalJPS(double weight, int BT, int SL);

protected:
    virtual bool
    is_pruned_neigbour(const vertex &x, const vertex &neighbour, int direction, const Map &map) const override;

    bool is_vertical_direction(int direction) const;

    bool is_horizontal_direction(int direction) const;

    virtual std::pair<vertex, bool> jump(vertex position, int direction, const vertex &goal,
                                         const EnvironmentOptions &options, const Map &map) const override;

};

#endif //ASEARCH_CUTTED_JPS_H
