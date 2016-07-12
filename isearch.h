#ifndef ISEARCH_H
#define ISEARCH_H
#include "list.h"
#include "map.h"
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include <unordered_map>
#include <math.h>
class ISearch
{
    public:
        ISearch();
        virtual ~ISearch(void);
        SearchResult startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options);

    protected:
        SearchResult    sresult;
        int     sizelimit; //����������� �� ������ OPEN
        float   hweight; //��� ���������
        int     breakingties; //�������� ������ ��������� ������� �� OPEN, ����� F-�������� �����

        virtual double heuristic(int i, int j, int goal_i, int goal_j, const EnvironmentOptions &options) const = 0;

};
#endif
