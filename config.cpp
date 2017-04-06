#include "config.h"
#include "gl_const.h"
#include "tinyxml2.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <string>

Config::Config() {
    LogParams = NULL;
    SearchParams = NULL;
}

Config::~Config() {
    if (SearchParams) delete[] SearchParams;
    if (LogParams) delete[] LogParams;
}

bool Config::getConfig(const char *FileName) {
    std::string value;
    std::stringstream stream;
    tinyxml2::XMLElement *root = 0, *algorithm = 0, *element = 0, *options = 0;

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }

    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' element found in XML file!" << std::endl;
        return false;
    }

    algorithm = root->FirstChildElement(CNS_TAG_ALG);
    if (!algorithm) {
        std::cout << "Error! No '" << CNS_TAG_ALG << "' tag found in XML file!" << std::endl;
        return false;
    }

    element = algorithm->FirstChildElement(CNS_TAG_ST);
    if (!element) {
        std::cout << "Error! No '" << CNS_TAG_ST << "' tag found in XML file!" << std::endl;
        return false;
    }
    if (element->GetText())
        value = element->GetText();
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);

    if (value == CNS_SP_ST_BFS) {
        N = 6;
        SearchParams = new double[N];
        SearchParams[CN_SP_ST] = CN_SP_ST_BFS;
    } else if (value == CNS_SP_ST_DIJK) {
        N = 6;
        SearchParams = new double[N];
        SearchParams[CN_SP_ST] = CN_SP_ST_DIJK;
    } else if (value == CNS_SP_ST_ASTAR || value == CNS_SP_ST_JP_SEARCH || value == CNS_SP_ST_TH) {
        N = 11;
        SearchParams = new double[N];
        SearchParams[CN_SP_ST] = CN_SP_ST_ASTAR;
        if (value == CNS_SP_ST_JP_SEARCH)
            SearchParams[CN_SP_ST] = CN_SP_ST_JP_SEARCH;
        if (value == CNS_SP_ST_TH)
            SearchParams[CN_SP_ST] = CN_SP_ST_TH;
        element = algorithm->FirstChildElement(CNS_TAG_HW);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_HW << "' tag found in algorithm section." << std::endl;
            std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1." << std::endl;
            SearchParams[CN_SP_HW] = 1;
        } else {
            stream << element->GetText();
            stream >> SearchParams[CN_SP_HW];
            stream.str("");
            stream.clear();

            if (SearchParams[CN_SP_HW] < 1) {
                std::cout << "Warning! Value of '" << CNS_TAG_HW << "' tag is not correctly specified. Should be >= 1."
                          << std::endl;
                std::cout << "Value of '" << CNS_TAG_HW << "' was defined to 1." << std::endl;
                SearchParams[CN_SP_HW] = 1;
            }
        }

        element = algorithm->FirstChildElement(CNS_TAG_MT);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_MT << "' tag found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'." << std::endl;
            SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
        } else {
            if (element->GetText())
                value = element->GetText();
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if (value == CNS_SP_MT_MANH) SearchParams[CN_SP_MT] = CN_SP_MT_MANH;
            else if (value == CNS_SP_MT_EUCL) SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
            else if (value == CNS_SP_MT_DIAG) SearchParams[CN_SP_MT] = CN_SP_MT_DIAG;
            else if (value == CNS_SP_MT_CHEB) SearchParams[CN_SP_MT] = CN_SP_MT_CHEB;
            else {
                std::cout << "Warning! Value of'" << CNS_TAG_MT << "' is not correctly specified." << std::endl;
                std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'" << std::endl;
                SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
            }
            if (SearchParams[CN_SP_ST] == CN_SP_ST_TH && SearchParams[CN_SP_MT] != CN_SP_MT_EUCL) {
                std::cout << "Warning! Theta* cannot work with this type of metric!" << std::endl;
                std::cout << "Value of '" << CNS_TAG_MT << "' was defined to 'euclidean'" << std::endl;
                SearchParams[CN_SP_MT] = CN_SP_MT_EUCL;
            }
        }


        element = algorithm->FirstChildElement(CNS_TAG_BT);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_BT << "' tag found in XML file" << std::endl;
            std::cout << "Value of '" << CNS_TAG_BT << "' was defined to 'g-max'" << std::endl;
            SearchParams[CN_SP_BT] = CN_SP_BT_GMAX;
        } else {
            value = element->GetText();
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);

            if (value == CNS_SP_BT_GMIN) SearchParams[CN_SP_BT] = CN_SP_BT_GMIN;
            else if (value == CNS_SP_BT_GMAX) SearchParams[CN_SP_BT] = CN_SP_BT_GMAX;
            else {
                std::cout << "Warning! Value of '" << CNS_TAG_BT << "' is not correctly specified." << std::endl;
                std::cout << "Value of '" << CNS_TAG_BT << "' was defined to 'g-max'" << std::endl;
                SearchParams[CN_SP_BT] = CN_SP_BT_GMAX;
            }
        }

        element = algorithm->FirstChildElement(CNS_TAG_SL);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_SL << "' element found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_SL << "' was defined to default - " << CN_SP_SL_NOLIMIT << std::endl;
            SearchParams[CN_SP_SL] = CN_SP_SL_NOLIMIT;
        } else {
            stream << element->GetText();
            stream >> SearchParams[CN_SP_SL];
            stream.str("");
            stream.clear();

            if (SearchParams[CN_SP_SL] == 0) {
                std::cout << "Warning! Value of '" << CNS_TAG_SL << "' is not correctly specified." << std::endl;
                std::cout << "Value of '" << CNS_TAG_SL << "' was defined to default - " << CN_SP_SL_NOLIMIT
                          << std::endl;
                SearchParams[CN_SP_SL] = CN_SP_SL_NOLIMIT;
            }
        }

    } else {
        std::cout << "Error! Value of '" << CNS_TAG_ST << "' tag (algorithm name) is not correctly specified."
                  << std::endl;
        std::cout << "Supported algorithm's names are: '" << CNS_SP_ST_BFS << "', '" << CNS_SP_ST_DIJK << "', '"
                  << CNS_SP_ST_ASTAR << "', '" << CNS_SP_ST_TH << "'." << std::endl;
        return false;
    }

    element = algorithm->FirstChildElement(CNS_TAG_AS);
    if (!element) {
        std::cout << "Warning! No '" << CNS_TAG_AS << "' element found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_AS << "' was defined to default - 0." << std::endl;

        SearchParams[CN_SP_AS] = CN_SP_AS_FALSE;
    } else {
        std::string check;
        stream << CN_SP_AS_FALSE;
        stream >> check;
        stream.clear();
        stream.str("");
        stream << element->GetText();
        stream >> SearchParams[CN_SP_AS];
        stream.clear();
        stream.str("");
        if (SearchParams[CN_SP_AS] != CN_SP_AS_TRUE && check != element->GetText()) {
            std::cout << "Warning! Value of '" << CNS_TAG_AS << "' is not correctly specified." << std::endl;
            std::cout << "Value of '" << CNS_TAG_AS << "' was defined to default - 0." << std::endl;
            SearchParams[CN_SP_AS] = CN_SP_AS_FALSE;
        }
    }

    element = algorithm->FirstChildElement(CNS_TAG_LC);
    if (!element) {
        std::cout << "Warning! No '" << CNS_TAG_LC << "' element found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_LC << "' was defined to default - " << CN_MC_LINE << std::endl;

        SearchParams[CN_SP_LC] = CN_MC_LINE;
    } else {
        stream << element->GetText();
        stream >> SearchParams[CN_SP_LC];
        stream.str("");
        stream.clear();

        if (SearchParams[CN_SP_LC] <= 0) {
            std::cout << "Warning! Value of '" << CNS_TAG_LC << "' is not correctly specified." << std::endl;
            std::cout << "Value of '" << CNS_TAG_LC << "' was defined to default - " << CN_MC_LINE << std::endl;
            SearchParams[CN_SP_LC] = CN_MC_LINE;
        }
        SearchParams[CN_SP_DC] = SearchParams[CN_SP_LC] * sqrt(2);
    }

    element = algorithm->FirstChildElement(CNS_TAG_RP);
    if (!element) {
        std::cout << "Warning! No '" << CNS_TAG_RP << "' element found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_RP << "' was defined to default - " << CN_SP_AS_FALSE << std::endl;

        SearchParams[CN_SP_RP] = CN_SP_AS_FALSE;
    } else {
        std::string check;
        stream << CN_SP_AS_FALSE;
        stream >> check;
        stream.clear();
        stream.str("");
        stream << element->GetText();
        stream >> SearchParams[CN_SP_RP];
        stream.clear();
        stream.str("");
        if (SearchParams[CN_SP_RP] != CN_SP_AS_TRUE && check != element->GetText()) {
            std::cout << "Warning! Value of '" << CNS_TAG_RP << "' is not correctly specified." << std::endl;
            std::cout << "Value of '" << CNS_TAG_RP << "' was defined to default - 0" << std::endl;
            SearchParams[CN_SP_RP] = CN_SP_AS_TRUE;
        }
    }
    SearchParams[CN_SP_DC] = SearchParams[CN_SP_LC] * sqrt(2);

    //TODO парсить diagonalcost с проверкой (в случае Theta*), что diagonalcost = sqrt(2) * linecost
    /*element = algorithm -> FirstChildElement(CNS_TAG_DC);
    if (!element)
    {
        std::cout << "Warning! No '"<< CNS_TAG_DC <<"' element found in XML file." << std::endl;
        std::cout << "Value of '"<< CNS_TAG_DC <<"' was defined to default - "<< CN_MC_DIAG << std::endl;
        SearchParams[CN_SP_DC] = CN_MC_DIAG;
    }
    else
    {
        stream << element->GetText();
        stream >> SearchParams[CN_SP_DC];
        stream.str("");
        stream.clear();

        if (SearchParams[CN_SP_DC]<=0)
        {
            std::cout << "Warning! Value of '" << CNS_TAG_DC <<"' is not correctly specified." << std::endl;
            std::cout << "Value of '"<< CNS_TAG_DC <<"' was defined to default - "<< CN_MC_DIAG << std::endl;
            SearchParams[CN_SP_DC] = CN_MC_DIAG;
            if(SearchParams[CN_SP_ST]==CN_SP_ST_TH)
                SearchParams[CN_SP_DC]=SearchParams[CN_SP_LC]*sqrt(2);
        }
    }

    if(SearchParams[CN_SP_DC]<=SearchParams[CN_SP_LC])
    {
        std::cout << "Warning! Value of '" << CNS_TAG_DC << "' should be greater than value of '"<< CNS_TAG_LC <<"'." << std::endl;
        std::cout << "Value of '"<< CNS_TAG_DC <<"' was defined to default - "<< CN_MC_DIAG << std::endl;
        std::cout << "Value of '"<< CNS_TAG_LC <<"' was defined to default - "<< CN_MC_LINE << std::endl;
        SearchParams[CN_SP_LC] = CN_MC_LINE;
        SearchParams[CN_SP_DC] = CN_MC_DIAG;
    }*/

    element = algorithm->FirstChildElement(CNS_TAG_AD);
    if (!element) {
        std::cout << "Warning! No '" << CNS_TAG_AD << "' element found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_AD << "' was defined to default - " << CN_SP_AD_TRUE << std::endl;
        SearchParams[CN_SP_AD] = CN_SP_AD_TRUE;
    } else {
        std::string check;
        stream << CN_SP_AD_FALSE;
        stream >> check;
        stream.clear();
        stream.str("");
        stream << element->GetText();
        stream >> SearchParams[CN_SP_AD];
        stream.str("");
        stream.clear();
        if (SearchParams[CN_SP_AD] != CN_SP_AD_TRUE && check != element->GetText()) {
            std::cout << "Warning! Value of '" << CNS_TAG_AD << "' is not correctly specified." << std::endl;
            std::cout << "Value of '" << CNS_TAG_AD << "' was defined to default - " << CN_SP_AD_TRUE << std::endl;
            SearchParams[CN_SP_AD] = CN_SP_AD_TRUE;
        }
        if (SearchParams[CN_SP_ST] == CN_SP_ST_TH && SearchParams[CN_SP_AD] != CN_SP_AD_TRUE) {
            std::cout << "Warning! Theta* cannot work without diagonal movements!" << std::endl;
            std::cout << "Value of '" << CNS_TAG_AD << "' was defined to '1'" << std::endl;
            SearchParams[CN_SP_AD] = CN_SP_AD_TRUE;
        }
    }

    element = algorithm->FirstChildElement(CNS_TAG_AC);
    if (!element) {
        std::cout << "Warning! No '" << CNS_TAG_AC << "' element found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_AC << "' was defined to default - " << CN_SP_AC_TRUE << std::endl;
        SearchParams[CN_SP_AC] = CN_SP_AC_TRUE;
    } else {
        std::string check;
        stream << CN_SP_AC_FALSE;
        stream >> check;
        stream.clear();
        stream.str("");
        stream << element->GetText();
        stream >> SearchParams[CN_SP_AC];
        stream.str("");
        stream.clear();
        if (SearchParams[CN_SP_AC] != CN_SP_AC_TRUE && check != element->GetText()) {
            std::cout << "Warning! Value of '" << CNS_TAG_AC << "' is not correctly specified." << std::endl;
            std::cout << "Value of '" << CNS_TAG_AC << "' was defined to default - " << CN_SP_AC_TRUE << std::endl;
            SearchParams[CN_SP_AC] = CN_SP_AC_TRUE;
        }
    }

    options = root->FirstChildElement(CNS_TAG_OPT);
    LogParams = new std::string[2];
    LogParams[CN_LP_LPATH] = "";
    LogParams[CN_LP_LNAME] = "";
    if (!options) {
        std::cout << "Warning! No '" << CNS_TAG_OPT << "' tag found in XML file." << std::endl;
        std::cout << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1)." << std::endl;
        SearchParams[CN_SP_LL] = CN_SP_LL_SMALLLOG;
    } else {
        element = options->FirstChildElement(CNS_TAG_LOGLVL);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_LOGLVL << "' tag found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1)." << std::endl;
            SearchParams[CN_SP_LL] = CN_SP_LL_SMALLLOG;
        } else {
            double loglevel;
            value = element->GetText();
            std::transform(value.begin(), value.end(), value.begin(), std::tolower);

            try {
                loglevel = std::stod(value);
            } catch (std::invalid_argument) {
                loglevel = -1;
            }

            int is_converted = 0;
            if (loglevel == CN_SP_LL_NOLOG || value == CN_SP_LL_NOLOG_WORD) {
                SearchParams[CN_SP_LL] = CN_SP_LL_NOLOG;
                is_converted = 1;
            } else if (loglevel == CN_SP_LL_SMALLLOG || value == CN_SP_LL_SMALLLOG_WORD) {
                SearchParams[CN_SP_LL] = CN_SP_LL_SMALLLOG;
                is_converted = 1;
            } else if (loglevel == CN_SP_LL_FULLLOG || value == CN_SP_LL_FULLLOG_WORD) {
                SearchParams[CN_SP_LL] = CN_SP_LL_FULLLOG;
                is_converted = 1;
            } else if (loglevel == CN_SP_LL_PARTIALLOG || value == CN_SP_LL_PARTIALLOG_WORD) {
                SearchParams[CN_SP_LL] = CN_SP_LL_PARTIALLOG;
                is_converted = 1;
            } else if (loglevel == CN_SP_LL_TINY || value == CN_SP_LL_TINY_WORD) {
                SearchParams[CN_SP_LL] = CN_SP_LL_TINY;
                is_converted = 1;
            }
            if (!is_converted) {
                std::cout << "'" << CNS_TAG_LOGLVL << "' is not correctly specified" << '\n'
                          << "Available logging value are: " << CN_SP_LL_SMALLLOG << " (short log), "
                          << CN_SP_LL_PARTIALLOG
                          << " (partial log, prints only final states of opened and closed lists), "
                          << CN_SP_LL_FULLLOG
                          << " (full log, prints state of of opened and closed lists on every step)\n"
                          << "Value of '" << CNS_TAG_LOGLVL << "' tag was defined to 'short log' (1)." << std::endl;
                SearchParams[CN_SP_LL] = CN_SP_LL_SMALLLOG;
            }
        }


        element = options->FirstChildElement(CNS_TAG_LOGPATH);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_LOGPATH << "' tag found in XML file." << std::endl;
            std::cout << "Value of '" << CNS_TAG_LOGPATH << "' tag was defined to 'current directory'." << std::endl;
        } else if (!element->GetText()) {
            std::cout << "Warning! Value of '" << CNS_TAG_LOGPATH << "' tag is missing!" << std::endl;
            std::cout << "Value of '" << CNS_TAG_LOGPATH << "' tag was defined to 'current directory'." << std::endl;
        } else {
            LogParams[CN_LP_LPATH] = element->GetText();
        }


        element = options->FirstChildElement(CNS_TAG_LOGFN);
        if (!element) {
            std::cout << "Warning! No '" << CNS_TAG_LOGFN << "' tag found in XML file!" << std::endl;
            std::cout << "Value of '" << CNS_TAG_LOGFN
                      << "' tag was defined to default (original filename +'_log' + original file extension."
                      << std::endl;
        } else if (!element->GetText()) {
            std::cout << "Warning! Value of '" << CNS_TAG_LOGFN << "' tag is missing." << std::endl;
            std::cout << "Value of '" << CNS_TAG_LOGFN
                      << "' tag was defined to default (original filename +'_log' + original file extension."
                      << std::endl;
        } else {
            LogParams[CN_LP_LNAME] = element->GetText();
        }
    }

    return true;
}
