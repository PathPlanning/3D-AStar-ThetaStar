#ifndef THETA_H
#define THETA_H
#include "gl_const.h"
#include "astar.h"

class Theta: public Astar
{
    public:
        Theta(float hweight, int breakingties, int sizelimit, int i):Astar(hweight,breakingties, sizelimit, i){}
        ~Theta(void);


    private:
        const double computation_eps = 0.0001;
    protected:
        double EuclidDistance(const Node &from, const Node &to, const EnvironmentOptions &options) const;
        Node resetParent(Node current, Node parent, const Map &map, const EnvironmentOptions &options);
        void makePrimaryPath(Node curNode);
        void makeSecondaryPath(const Map &map,Node curNode);


};


#endif // THETA_H
