#ifndef ISEARCH_H
#define ISEARCH_H

#include "list.h"
#include "map.h"
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include "node.h"
#include "prior_queue.h"

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

    virtual double heuristic(int i, int j, int goal_i, int goal_j, const EnvironmentOptions &options) const;

    virtual std::list<Node> find_successors(Node *current, const Map &map, const EnvironmentOptions &options) const;

    virtual void improve_parent(Node *current, const Map &map, const EnvironmentOptions &options) const;

    virtual void recovery_primary_path(Node *finish, int start_i, int start_j) = 0;

    virtual void recovery_secondary_path(Node *finish, int start_i, int start_j, const EnvironmentOptions &options) = 0;

    /*
    * Returns direction between point according to gl_const
    */
    int direction(const vertex &from, const vertex &to) const;

    int direction(int delta_i, int delta_j) const;

    int direction(const Node &from, const Node &to) const;
};

#endif
