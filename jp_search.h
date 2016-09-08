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
    bool findNeighbors(int move_i, int move_j, Node curNode, const Map &map);//����� ����������� ��� ���������� ��������� ����� ��� �������� �� ���������
    void findJP(int move_i, int move_j, Node curNode, const Map &map, std::list<Node> &successors, const EnvironmentOptions &options);//�����, ������� ���� ��������� �����; successors �� const �.�. � ���� � ����� ������������ ��������� �����
    int findDirection(int current_i, int parent_i);//���������� ����������� ��������
    std::list<Node> findSuccessors(Node curNode, const Map &map, const EnvironmentOptions &options);
    Node resetParent(Node current, Node parent, const Map &map, const EnvironmentOptions &options);
    bool lineOfSign(int parent_i, int parent_j, int curent_i, int curent_j, const Map &map);//��������� ������������� ����� ����� ����� �������
    bool grid(int i, int j, const Map &map);//��������� �������������� ����� � ����� � ������������
    void makePrimaryPath(Node curNode);
    void makeSecondaryPath(const Map &map,Node curNode);
    void findBlockObstacle(const Map &map){return;}
};

#endif // JP_SEARCH_H
