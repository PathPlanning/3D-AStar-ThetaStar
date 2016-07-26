#ifndef HEURISTICSEARCH_THETA_STAR_H
#define HEURISTICSEARCH_THETA_STAR_H


#include "isearch.h"

class Theta_star : public ISearch {
public:
    Theta_star(double hweight, int BT, int SL);

protected:
    virtual void add_successors_to_opened(const extNode &pos, minqueue &opened,
                                          const std::map<vertex, std::pair<double, vertex>> &closed,
                                          const EnvironmentOptions &options, const Map &map) override;
};


#endif //HEURISTICSEARCH_THETA_STAR_H
