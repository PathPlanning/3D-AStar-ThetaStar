#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include "list.h"
#include <ctime>
#include <map>
#include <set>

struct SearchResult
{
        bool pathfound; //путь найден
        double pathlength; //длина пути, если путь не найден, то равен 0
        const NodeList* lppath; //путь в виде последовательности вершин
        const NodeList* hppath; //путь разбитый на секции
        unsigned int nodescreated; //|OPEN| + |CLOSE| = число созданных нодов (= показатель емкостной эффективности поиска)
        unsigned int numberofsteps; //число итераций (= показатель временной эффективности поиска)
        double time;//фактически затраченное время (= "абсолютный" (но зависящий от конкретной реализации) показатель временной эффективности поиска)
        const std::map<Node, bool>* viewed; // Просмотренны вершины. Булевое значение показывает, была ли вершина в CLOSED или OPENED списке
    SearchResult()
        {
            pathfound = false;
            pathlength = 0;
            lppath = NULL;
            hppath = NULL;
            nodescreated = 0;
            numberofsteps = 0;
            time = 0;
        }

};

#endif
