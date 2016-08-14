#ifndef HEURISTICSEARCH_THETA_STAR_H
#define HEURISTICSEARCH_THETA_STAR_H


#include "isearch.h"

class Theta_star : public ISearch {
public:
    Theta_star(double hweight, int BT, int SL);

protected:
    double euclid_distance(const vertex &from, const vertex &to, double linecost) const;

    double euclid_distance(const Node &from, const Node &to, double linecost) const;

    bool line_of_sight(const Node &from, const Node &to, const Map &map) const;

    void construct_line(const Node &from, const Node &to, NodeList &out, const EnvironmentOptions &options) const;

    virtual void improve_parent(Node *current, const Map &map, const EnvironmentOptions &options) const;

    virtual void recovery_primary_path(Node *finish, int start_i, int start_j);

    virtual void recovery_secondary_path(Node *finish, int start_i, int start_j, const EnvironmentOptions &options);
};


#endif //HEURISTICSEARCH_THETA_STAR_H
