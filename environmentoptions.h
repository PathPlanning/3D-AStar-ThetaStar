#ifndef ENVIRONMENTOPTIONS_H
#define ENVIRONMENTOPTIONS_H

class EnvironmentOptions
{
public:
    EnvironmentOptions(int MT, bool AS, double LC, double DC, int AD);
    EnvironmentOptions();
    int    metrictype; //��� ������� ��� �������� ���������
    bool   allowsqueeze;//����� �����������/����������� ������ ����� "����� �������"
    double  linecost; //��������� ����������� �� ����������� � ���������
    double  diagonalcost; //��������� ������������ �� ���������
    int    allowdiagonal; //����� �����������/����������� ����������� �� ���������
    bool   useresetparent; //����� ������������/����������� ������������� ������ resetparent � jp_search

};

#endif // ENVIRONMENTOPTIONS_H
