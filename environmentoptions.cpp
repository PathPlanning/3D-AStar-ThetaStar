#include "environmentoptions.h"
#include "gl_const.h"

EnvironmentOptions::EnvironmentOptions()
{
    metrictype = CN_SP_MT_EUCL;
    allowsqueeze = CN_SP_AS_FALSE;
    linecost = CN_MC_LINE;
    diagonalcost = CN_MC_DIAG;
    allowdiagonal = CN_SP_AD_TRUE;
    allowcutcorners = CN_SP_AC_FALSE;
}

EnvironmentOptions::EnvironmentOptions(int MT, bool AS, double LC, double DC, int AD, bool AC)
{
    metrictype = MT;
    allowsqueeze = AS;
    linecost = LC;
    diagonalcost = DC;
    allowdiagonal = AD;
    allowcutcorners = AC;
}

