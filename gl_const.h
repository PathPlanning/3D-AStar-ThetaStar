#ifndef GL_CONST_H
#define    GL_CONST_H

/*
 * XML file tags ---------------------------------------------------------------
 */
#define CNS_TAG_ROOT "root"

#define CNS_TAG_MAP         "map"
#define CNS_TAG_WIDTH   "width"
#define CNS_TAG_HEIGHT  "height"
#define CNS_TAG_STX     "startx"
#define CNS_TAG_STY     "starty"
#define CNS_TAG_STZ     "startz"
#define CNS_TAG_FINX    "finishx"
#define CNS_TAG_FINY    "finishy"
#define CNS_TAG_FINZ    "finishz"
#define CNS_TAG_GRID    "grid"
#define CNS_TAG_ROW "row"
#define CNS_TAG_MAXALT         "maxaltitude"
#define CNS_TAG_ALTLIM         "altitudelimits"
    #define CNS_TAG_ALTLIM_ATTR_MIN "min"
    #define CNS_TAG_ALTLIM_ATTR_MAX "max"

#define CNS_TAG_ALG     "algorithm"
#define CNS_TAG_ST  "searchtype"
#define CNS_TAG_HW  "hweight"
#define CNS_TAG_MT  "metrictype"
#define CNS_TAG_BT  "breakingties"
#define CNS_TAG_SL  "sizelimit"
#define CNS_TAG_AS  "allowsqueeze"
#define CNS_TAG_LC  "linecost"
#define CNS_TAG_DC  "diagonalcost"
#define CNS_TAG_AD  "allowdiagonal"
#define CNS_TAG_AC  "cutcorners"
#define CNS_TAG_RP  "useresetparent"

#define CNS_TAG_OPT         "options"
#define CNS_TAG_LOGLVL  "loglevel"
#define CNS_TAG_LOGPATH "logpath"
#define CNS_TAG_LOGFN   "logfilename"

#define CNS_TAG_LOG "log"
#define CNS_TAG_MAPFN       "mapfilename"
#define CNS_TAG_SUM         "summary"
#define CNS_TAG_PATH        "path"
#define CNS_TAG_LPLEVEL     "lplevel"
#define CNS_TAG_HPLEVEL     "hplevel"
#define CNS_TAG_SECTION "section"
#define CNS_TAG_LOWLEVEL    "lowlevel"
#define CNS_TAG_STEP    "step"
#define CNS_TAG_OPEN    "open"
#define CNS_TAG_POINT   "node"
#define CNS_TAG_CLOSE   "close"

/*
 * End of XML files tags -------------------------------------------------------
 */

/*
 * XML files tag's attributes --------------------------------------------------
 */
#define CNS_TAG_ATTR_NUMOFSTEPS     "numberofsteps"
#define CNS_TAG_ATTR_NODESCREATED   "nodescreated"
#define CNS_TAG_ATTR_LENGTH         "length"
#define CNS_TAG_ATTR_TIME           "time"
#define CNS_TAG_ATTR_X              "x"
#define CNS_TAG_ATTR_Y              "y"
#define CNS_TAG_ATTR_Z              "z"
#define CNS_TAG_ATTR_NUM            "number"
#define CNS_TAG_ATTR_F              "F"
#define CNS_TAG_ATTR_G              "g"
#define CNS_TAG_ATTR_PARX           "parent_x"
#define CNS_TAG_ATTR_PARY           "parent_y"
#define CNS_TAG_ATTR_PARZ           "parent_z"
#define CNS_TAG_ATTR_STX            "start.x"
#define CNS_TAG_ATTR_STY            "start.y"
#define CNS_TAG_ATTR_STZ            "start.z"
#define CNS_TAG_ATTR_FINX           "finish.x"
#define CNS_TAG_ATTR_FINY           "finish.y"
#define CNS_TAG_ATTR_FINZ           "finish.z"


/*
 * End of XML files tag's attributes -------------------------------------------
 */

/*
 * Configuration. SearchParams array -------------------------------------------
 */
#define CN_SP_ST 0

#define CNS_SP_ST_BFS       "bfs"
#define CNS_SP_ST_JP_SEARCH "jp_search"
#define CNS_SP_ST_DIJK      "dijkstra"
#define CNS_SP_ST_ASTAR     "astar"
#define CNS_SP_ST_TH        "theta"

#define CN_SP_ST_BFS        0
#define CN_SP_ST_DIJK       1
#define CN_SP_ST_ASTAR      2
#define CN_SP_ST_JP_SEARCH  3
#define CN_SP_ST_TH         4


#define CN_SP_LL 1

#define CN_SP_LL_NOLOG      0
#define CN_SP_LL_TINY       0.5
#define CN_SP_LL_SMALLLOG   1
#define CN_SP_LL_FULLLOG    2
#define CN_SP_LL_PARTIALLOG    1.5

#define CN_SP_LL_NOLOG_WORD "none"
#define CN_SP_LL_TINY_WORD "tiny"
#define CN_SP_LL_SMALLLOG_WORD   "short"
#define CN_SP_LL_FULLLOG_WORD    "medium"
#define CN_SP_LL_PARTIALLOG_WORD    "full"

#define CN_SP_AS 2 //AllowSqueeze

#define CN_SP_AS_TRUE 1
#define CN_SP_AS_FALSE 0

#define CN_SP_AC_TRUE 1
#define CN_SP_AC_FALSE 0

#define CN_SP_LC 3 //LineCost

#define CN_SP_DC 4 //DiagonalCost

#define CN_SP_AD 5 //AllowDiagonal

#define CN_SP_AC 6 //AllowCutcorners
#define CN_SP_AD_TRUE 1
#define CN_SP_AD_FALSE 0

#define CN_SP_HW 6 //HWeight

#define CN_SP_MT 7 //MetricType

#define CNS_SP_MT_DIAG "diagonal"
#define CNS_SP_MT_MANH "manhattan"
#define CNS_SP_MT_EUCL "euclid"
#define CNS_SP_MT_CHEB "chebyshev"

#define CN_SP_MT_DIAG 0
#define CN_SP_MT_MANH 1
#define CN_SP_MT_EUCL 2
#define CN_SP_MT_CHEB 3

#define CN_SP_BT 8

#define CNS_SP_BT_GMIN "g-min"
#define CNS_SP_BT_GMAX "g-max"

#define CN_SP_BT_GMIN 1
#define CN_SP_BT_GMAX 2

#define CN_SP_SL 9
#define CN_SP_SL_NOLIMIT -1 //����� ����� ��������� ���� ������ ���� ����� ����!

#define CN_SP_RP 10 //UseResetParent


#define CN_LP_LPATH 0
#define CN_LP_LNAME 1
/*
 * End Configuration -----------------------------------------------------------
 */

/*
* Move Cost -------------------------------------------------------------------
*/
#define CN_MC_LINE 10
#define CN_MC_DIAG 14
/*
* End of Move Cost ------------------------------------------------------------
*/

/*
 * Grid Cell -------------------------------------------------------------------
 */
#define CN_GC_NOOBS 0 //������ ���������
//#define CN_GC_OBS   1 //����������
/*
 * End of Grid Cell ------------------------------------------------------------
 */

/*
 * Other -----------------------------------------------------------------------
 */
#define CNS_OTHER_PATHSELECTION "*"
#define CNS_OTHER_MATRIXSEPARATOR ' '
#define CN_OTHER_GVALUEOFNOWAY -1
/*
 * End of other ----------------------------------------------------------------
 */
#endif

