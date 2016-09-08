#ifndef JP_SEARCH_H
#define JP_SEARCH_H
#include "gl_const.h"
#include "astar.h"
#include "ilogger.h"
#include <iostream>

class JP_Search:public Astar
{
public:
    JP_Search(float hweight, int breakingties, int sizelimit, int i):Astar(hweight,breakingties,sizelimit, i){/*open=new NodeList[i];*/}
    ~JP_Search();

private:
    bool findNeighbors(int move_i, int move_j, Node curNode, const Map &map);//метод необходимый для нахождения прыжковой точки при движении по диагонали
    void findJP(int move_i, int move_j, Node curNode, const Map &map, std::list<Node> &successors, const EnvironmentOptions &options);//метод, который ищет прыжковую точку; successors не const т.к. в него и будет записываться прыжковая точка
    int findDirection(int current_i, int parent_i);//определяет направление движения
    std::list<Node> findSuccessors(Node curNode, const Map &map, const EnvironmentOptions &options);
    Node resetParent(Node current, Node parent, const Map &map, const EnvironmentOptions &options);
    bool lineOfSign(int parent_i, int parent_j, int curent_i, int curent_j, const Map &map);//проверяет существование линии между двумя точками
    bool grid(int i, int j, const Map &map);//проверяет принадлежность точки к карте и проходимость
    void makePrimaryPath(Node curNode);
    void makeSecondaryPath(const Map &map,Node curNode);
    void findBlockObstacle(const Map &map){return;}
};

#endif // JP_SEARCH_H
