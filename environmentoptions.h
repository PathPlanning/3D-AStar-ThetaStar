#ifndef ENVIRONMENTOPTIONS_H
#define ENVIRONMENTOPTIONS_H

class EnvironmentOptions
{
public:
    EnvironmentOptions(int MT, bool AS, double LC, double DC, int AD, bool AC);
    EnvironmentOptions();
    int    metrictype; //��� ������� ��� �������� ���������
    bool   allowsqueeze;//����� �����������/����������� ������ ����� "����� �������"
    bool   allowcutcorners;//����� �����������/����������� ������ ����� "����� �������"
    double  linecost; //��������� ����������� �� ����������� � ���������
    double  diagonalcost; //��������� ������������ �� ���������
    int    allowdiagonal; //����� �����������/����������� ����������� �� ���������
    bool   useresetparent; //����� ������������/����������� ������������� ������ resetparent � jp_search

};

#endif // ENVIRONMENTOPTIONS_H
