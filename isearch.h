#ifndef ISEARCH_H
#define ISEARCH_H

#include "list.h"
#include "map.h"
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include "Queues.h"

#include <unordered_set>
#include <unordered_map>

class ISearch {
public:
    ISearch();

    virtual ~ISearch(void);

    SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);

protected:
    Node findMin(int size);
    double MoveCost(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h, const EnvironmentOptions &options);
    void deleteMin(Node minNode);
    virtual void addOpen(Node newNode);
    virtual double computeHFromCellToCell(int start_i, int start_j, int start_h, int fin_i, int fin_j, int fin_h,
                                          const EnvironmentOptions &options) = 0;
    // Method which searches the expanded node successor, which satisfies search conditions
    virtual std::list<Node> findSuccessors(Node curNode, const Map &map, const EnvironmentOptions &options);
    virtual void makePrimaryPath(Node curNode);
    virtual void makeSecondaryPath(const Map &map,
                                   Node curNode);
    // Tries to changed node's parent on better one
    virtual Node resetParent(Node current, Node parent, const Map &map,
                             const EnvironmentOptions &options) { return current; }
    virtual bool stopCriterion();

    SearchResult sresult;
    NodeList lppath, hppath; // Found point by point and section paths
    std::unordered_map<uint_least32_t, Node> close;
    std::unordered_set<Node> *open;
    std::vector<Node> openMinimums;

    int openSize;
    float hweight; // Heuristic weight coefficient
    int breakingties; // ID of criterion which used for choosing between node with the same f-value

};

#endif
