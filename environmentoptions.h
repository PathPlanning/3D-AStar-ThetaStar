#ifndef ENVIRONMENTOPTIONS_H
#define ENVIRONMENTOPTIONS_H

class EnvironmentOptions
{
public:
    EnvironmentOptions(int MT, bool AS, double LC, double DC, int AD);
    EnvironmentOptions();
    int    metrictype; //тип метрики для подсчета эвристики
    bool   allowsqueeze;//опция разрешающая/запрещающая проход через "узкий коридор"
    double  linecost; //стоимость перемещения по горизонтали и вертикали
    double  diagonalcost; //стоимость перемещаения по диагонали
    int    allowdiagonal; //опция разрешающая/запрещающая перемещения по диагонали
    bool   useresetparent; //опция разрешаяющая/запрещающая использование метода resetparent в jp_search

};

#endif // ENVIRONMENTOPTIONS_H
