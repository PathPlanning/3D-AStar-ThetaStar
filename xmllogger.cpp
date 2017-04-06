#include "xmllogger.h"
#include "tinyxml2.h"
#include "gl_const.h"
#include "node.h"
#include <sstream>
#include <iostream>

using tinyxml2::XMLElement;
using tinyxml2::XMLNode;

bool XmlLogger::getLog(const char *FileName, const std::string *LogParams) {
    if (loglevel == CN_SP_LL_NOLOG) return true;

    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening input XML file" << std::endl;
        return false;
    }

    if (LogParams[CN_LP_LPATH] == "" && LogParams[CN_LP_LNAME] == "") {
        std::string str;
        str.append(FileName);
        size_t found = str.find_last_of(".");
        if (found != std::string::npos)
            str.insert(found, "_log");
        else
            str.append("_log");
        LogFileName.append(str);
    } else if (LogParams[CN_LP_LPATH] == "") {
        LogFileName.append(FileName);
        std::string::iterator it = LogFileName.end();
        while (*it != '\\')
            it--;
        it++;
        LogFileName.erase(it, LogFileName.end());
        LogFileName.append(LogParams[CN_LP_LNAME]);
    } else if (LogParams[CN_LP_LNAME] == "") {
        LogFileName.append(LogParams[CN_LP_LPATH]);
        if (*(--LogParams[CN_LP_LPATH].end()) != '\\') LogFileName.append("\\");
        std::string lfn;
        lfn.append(FileName);
        size_t found = lfn.find_last_of("\\");
        std::string str = lfn.substr(found);
        found = str.find_last_of(".");
        if (found != std::string::npos)
            str.insert(found, "_log");
        else
            str.append("_log");
        LogFileName.append(str);
    } else {
        LogFileName.append(LogParams[CN_LP_LPATH]);
        if (*(--LogParams[CN_LP_LPATH].end()) != '\\') LogFileName.append("\\");
        LogFileName.append(LogParams[CN_LP_LNAME]);
    }

    XMLElement *log, *root = doc.FirstChildElement(CNS_TAG_ROOT);

    if (!root) {
        std::cout << "No '" << CNS_TAG_ROOT << "' element found in XML file" << std::endl;
        std::cout << "Can not create log" << std::endl;
        return false;
    }

    root->InsertEndChild(doc.NewElement(CNS_TAG_LOG));

    root = (root->LastChild())->ToElement();

    if (loglevel == CN_SP_LL_SMALLLOG || loglevel == CN_SP_LL_FULLLOG || loglevel == CN_SP_LL_PARTIALLOG) {
        log = doc.NewElement(CNS_TAG_MAPFN);
        log->InsertEndChild(doc.NewText(FileName));
        root->InsertEndChild(log);

        root->InsertEndChild(doc.NewElement(CNS_TAG_SUM));

        root->InsertEndChild(doc.NewElement(CNS_TAG_PATH));

        root->InsertEndChild(doc.NewElement(CNS_TAG_LPLEVEL));

        root->InsertEndChild(doc.NewElement(CNS_TAG_HPLEVEL));
    }

    if (loglevel == CN_SP_LL_FULLLOG || loglevel == CN_SP_LL_PARTIALLOG) {
        root->InsertEndChild(doc.NewElement(CNS_TAG_LOWLEVEL));
    }

    return true;
}

void XmlLogger::saveLog() {
    if (loglevel == CN_SP_LL_NOLOG) return;
    doc.SaveFile(LogFileName.c_str());
}

void XmlLogger::writeToLogMap(const Map &map, const NodeList &path) {
    if (loglevel == CN_SP_LL_NOLOG || loglevel == CN_SP_LL_TINY) return;

    int iterate = 0;
    std::stringstream stream;
    std::string str;
    NodeList temp = path;

    XMLElement *mapTag = doc.FirstChildElement(CNS_TAG_ROOT);
    mapTag = mapTag->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_PATH);

    for (int i = 0; i < map.height; i++, iterate++) {
        XMLElement *element = doc.NewElement(CNS_TAG_ROW);
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);

        for (int j = 0; j < map.width; j++) {
            if (!temp.find(i, j)) {
                stream << map.Grid[i][j];
                str += stream.str();
                stream.str("");
                stream.clear();
            } else {
                str += CNS_OTHER_PATHSELECTION;
            }
            str += CNS_OTHER_MATRIXSEPARATOR;
        }

        element->InsertEndChild(doc.NewText(str.c_str()));
        mapTag->InsertEndChild(element);
        str.clear();
    }
}

void XmlLogger::writeToLogOpenClose(const NodeList *open, const std::unordered_set<Node> &close, int size,
                                    bool summary = false) {

    if (loglevel == CN_SP_LL_NOLOG || loglevel == CN_SP_LL_TINY || loglevel == CN_SP_LL_SMALLLOG ||
        (loglevel == CN_SP_LL_PARTIALLOG && !summary))
        return;
    int iterate = 1;
    XMLElement *element = doc.NewElement(CNS_TAG_STEP);
    XMLElement *child , *lowlevel = doc.FirstChildElement(CNS_TAG_ROOT);
    lowlevel = lowlevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_LOWLEVEL);
    child = lowlevel->FirstChildElement();

    while (child = child->NextSiblingElement()) iterate++;

    element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);
    lowlevel->InsertEndChild(element);
    lowlevel = lowlevel->LastChildElement();

    lowlevel->InsertEndChild(doc.NewElement(CNS_TAG_OPEN));
    child = lowlevel->LastChildElement();

    Node min;
    min.F = -1;
    int exc = 0;
    for (int i = 0; i < size; i++) {
        if (open[i].List.size() > 0) {
            if (open[i].List.begin()->F <= min.F || min.F == -1) {
                if (open[i].List.begin()->F == min.F && open[i].List.begin()->g > min.g) {
                    min = *open[i].List.begin();
                    exc = i;
                } else if (open[i].List.begin()->F < min.F || min.F == -1) {
                    min = *open[i].List.begin();
                    exc = i;
                }
            }
        }
    }
    if (min.F != -1) {
        element = doc.NewElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, min.j);
        element->SetAttribute(CNS_TAG_ATTR_Y, min.i);
        element->SetAttribute(CNS_TAG_ATTR_F, min.F);
        element->SetAttribute(CNS_TAG_ATTR_G, min.g);
        if (min.g > 0) {
            element->SetAttribute(CNS_TAG_ATTR_PARX, min.parent->j);
            element->SetAttribute(CNS_TAG_ATTR_PARY, min.parent->i);
        }
        child->InsertEndChild(element);
    }
    for (int i = 0; i < size; i++) {
        if (open[i].List.size() > 0) {
            for (std::list<Node>::const_iterator it = open[i].List.begin(); it != open[i].List.end(); ++it) {
                if (it != open[exc].List.begin()) {
                    element = doc.NewElement(CNS_TAG_POINT);
                    element->SetAttribute(CNS_TAG_ATTR_X, it->j);
                    element->SetAttribute(CNS_TAG_ATTR_Y, it->i);
                    element->SetAttribute(CNS_TAG_ATTR_F, it->F);
                    element->SetAttribute(CNS_TAG_ATTR_G, it->g);
                    if (it->g > 0) {
                        element->SetAttribute(CNS_TAG_ATTR_PARX, it->parent->j);
                        element->SetAttribute(CNS_TAG_ATTR_PARY, it->parent->i);
                    }
                    child->InsertEndChild(element);
                }
            }
        }
    }

    lowlevel->InsertEndChild(doc.NewElement(CNS_TAG_CLOSE));
    child = lowlevel->LastChildElement();

    for (auto it = close.begin(); it != close.end(); ++it) {
        element = doc.NewElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, it->j);
        element->SetAttribute(CNS_TAG_ATTR_Y, it->i);
        element->SetAttribute(CNS_TAG_ATTR_Z, it->z);
        element->SetAttribute(CNS_TAG_ATTR_F, it->F);
        element->SetAttribute(CNS_TAG_ATTR_G, it->g);
        if (it->g > 0) {
            element->SetAttribute(CNS_TAG_ATTR_PARX, it->parent->j);
            element->SetAttribute(CNS_TAG_ATTR_PARY, it->parent->i);
            element->SetAttribute(CNS_TAG_ATTR_PARZ, it->parent->z);
        }
        child->InsertEndChild(element);
    }
}

void XmlLogger::writeToLogPath(const NodeList &path) {
    if (loglevel == CN_SP_LL_NOLOG || loglevel == CN_SP_LL_TINY || path.List.size() == 0) return;
    int iterate = 0;
    XMLElement *lplevel = doc.FirstChildElement(CNS_TAG_ROOT);
    lplevel = lplevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_LPLEVEL);

    for (std::list<Node>::const_iterator it = path.List.begin(); it != path.List.end(); ++it, ++iterate) {
        XMLElement *element = doc.NewElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, it->j);
        element->SetAttribute(CNS_TAG_ATTR_Y, it->i);
        element->SetAttribute(CNS_TAG_ATTR_Z, it->z);
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);
        lplevel->InsertEndChild(element);
    }
}

void XmlLogger::writeToLogHPpath(const NodeList &hppath) {
    if (loglevel < CN_SP_LL_SMALLLOG || loglevel == CN_SP_LL_TINY || hppath.List.size() == 0) return;
    int partnumber = 0;
    XMLElement *hplevel = doc.FirstChildElement(CNS_TAG_ROOT);
    hplevel = hplevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_HPLEVEL);
    std::list<Node>::const_iterator iter = hppath.List.begin();
    std::list<Node>::const_iterator it = hppath.List.begin();

    while (iter != --hppath.List.end()) {
        XMLElement *part = doc.NewElement(CNS_TAG_SECTION);
        part->SetAttribute(CNS_TAG_ATTR_NUM, partnumber);
        part->SetAttribute(CNS_TAG_ATTR_STX, it->j);
        part->SetAttribute(CNS_TAG_ATTR_STY, it->i);
        part->SetAttribute(CNS_TAG_ATTR_STZ, it->z);
        iter++;
        part->SetAttribute(CNS_TAG_ATTR_FINX, iter->j);
        part->SetAttribute(CNS_TAG_ATTR_FINY, iter->i);
        part->SetAttribute(CNS_TAG_ATTR_FINZ, iter->z);
        part->SetAttribute(CNS_TAG_ATTR_LENGTH, iter->g - it->g);
        hplevel->LinkEndChild(part);
        it++;
        partnumber++;
    }
}

void XmlLogger::writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, float length, double time) {
    if (loglevel == CN_SP_LL_NOLOG) return;

    std::stringstream str;
    str << time;

    XMLElement *summary = doc.FirstChildElement(CNS_TAG_ROOT);
    summary = summary->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_SUM);
    XMLElement *element = summary->ToElement();
    element->SetAttribute(CNS_TAG_ATTR_NUMOFSTEPS, numberofsteps);
    element->SetAttribute(CNS_TAG_ATTR_NODESCREATED, nodescreated);
    element->SetAttribute(CNS_TAG_ATTR_LENGTH, length);
    element->SetAttribute(CNS_TAG_ATTR_TIME, str.str().c_str());
}

void XmlLogger::writeToLogNotFound() {
    if (loglevel == CN_SP_LL_NOLOG) return;
    XMLElement *node = doc.FirstChildElement(CNS_TAG_ROOT)->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_PATH);
    node->InsertEndChild(doc.NewText("Path NOT found!"));
}
