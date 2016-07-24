#ifndef ASEARCH_CUTTED_JPS_H
#define ASEARCH_CUTTED_JPS_H

#include "isearch.h"
#include "minqueue.h"

#include <utility>
#include <vector>

class JPS : public ISearch {
public:
    JPS(double weight, int BT, int SL);

private:
    std::pair<int, int> corner_directions(int direction) const;

    bool is_pruned_neigbour(const vertex &x, const vertex &neighbour, int direction, const Map &map) const;

    bool has_forced_neigbour(const vertex &x, int direction, bool allow_squeeze, const Map &map) const;

    std::pair<vertex, bool> jump(vertex position, int direction, const vertex &goal,
                                 const EnvironmentOptions &options, const Map &map) const;

protected:
    virtual void add_successors_to_opened(const extNode &pos,
                                          minqueue &opened,
                                          const EnvironmentOptions &options,
                                          const Map &map);

};

class orthogonalJPS : public ISearch {
public:
    orthogonalJPS(double weight, int BT, int SL);

private:
    bool is_pruned_neigbour(const vertex &x, const vertex &neighbour, int direction, const Map &map) const;

    bool is_vertical_direction(int direction) const;

    bool is_horizontal_direction(int direction) const;

    std::pair<vertex, bool> jump(vertex position, int direction, const vertex &goal,
                                 const EnvironmentOptions &options, const Map &map) const;

protected:
    virtual void add_successors_to_opened(const extNode &pos,
                                          minqueue &opened,
                                          const EnvironmentOptions &options,
                                          const Map &map) override;
};

#endif //ASEARCH_CUTTED_JPS_H
