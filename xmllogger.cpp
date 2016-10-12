#include "xmllogger.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "gl_const.h"
#include "node.h"
#include <sstream>
#include <iostream>

XmlLogger::~XmlLogger() {
    delete doc;
}

bool XmlLogger::getLog(const char *FileName, const std::string *LogParams) {
    if (loglevel == CN_SP_LL_NOLOG) return true;

    doc = new TiXmlDocument(FileName);
    if (!doc->LoadFile()) {
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

    TiXmlElement *log, *root = doc->FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "No '" << CNS_TAG_ROOT << "' element found in XML file" << std::endl;
        std::cout << "Can not create log" << std::endl;
        return false;
    }

    log = new TiXmlElement(CNS_TAG_LOG);
    root->InsertEndChild(*log);

    root = (root->LastChild())->ToElement();

    if (loglevel == CN_SP_LL_SMALLLOG || loglevel == CN_SP_LL_FULLLOG || loglevel == CN_SP_LL_PARTIALLOG) {
        log = new TiXmlElement(CNS_TAG_MAPFN);
        log->LinkEndChild(new TiXmlText(FileName));
        root->InsertEndChild(*log);

        log = new TiXmlElement(CNS_TAG_SUM);
        root->InsertEndChild(*log);

        log = new TiXmlElement(CNS_TAG_PATH);
        root->InsertEndChild(*log);

        log = new TiXmlElement(CNS_TAG_LPLEVEL);
        root->InsertEndChild(*log);

        log = new TiXmlElement(CNS_TAG_HPLEVEL);
        root->InsertEndChild(*log);
    }

    if (loglevel == CN_SP_LL_FULLLOG || loglevel == CN_SP_LL_PARTIALLOG) {
        log = new TiXmlElement(CNS_TAG_LOWLEVEL);
        root->InsertEndChild(*log);
    }

    return true;
}

void XmlLogger::saveLog() {
    if (loglevel == CN_SP_LL_NOLOG) return;
    doc->SaveFile(LogFileName.c_str());
}

void XmlLogger::writeToLogMap(const Map &map, const NodeList &path) {
    if (loglevel == CN_SP_LL_NOLOG) return;

    int iterate = 0;
    std::stringstream stream;
    std::string str;
    TiXmlElement *element;
    NodeList temp = path;

    TiXmlNode *mapTag = doc->FirstChild(CNS_TAG_ROOT);
    mapTag = mapTag->FirstChild(CNS_TAG_LOG)->FirstChild(CNS_TAG_PATH);

    for (int i = 0; i < map.height; i++, iterate++) {
        element = new TiXmlElement(CNS_TAG_ROW);
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

        element->LinkEndChild(new TiXmlText(str.c_str()));
        mapTag->InsertEndChild(*element);
        str.clear();
    }
}

void XmlLogger::writeToLogOpenClose(const NodeList *open, const std::unordered_set<Node> &close, int size,
                                    bool summary = false) {

    if (loglevel == CN_SP_LL_NOLOG || loglevel == CN_SP_LL_SMALLLOG ||
        (loglevel == CN_SP_LL_PARTIALLOG && !summary))
        return;
    int iterate = 0;
    TiXmlElement *element = new TiXmlElement(CNS_TAG_STEP);
    TiXmlNode *child = 0, *lowlevel = doc->FirstChild(CNS_TAG_ROOT);
    lowlevel = lowlevel->FirstChild(CNS_TAG_LOG)->FirstChild(CNS_TAG_LOWLEVEL);

    while (child = lowlevel->IterateChildren(child))
        iterate++;

    element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);
    lowlevel->InsertEndChild(*element);
    lowlevel = lowlevel->LastChild();

    element = new TiXmlElement(CNS_TAG_OPEN);
    lowlevel->InsertEndChild(*element);
    child = lowlevel->LastChild();

    Node min;
    min.F = -1;
    int exc = 0;
    for (int i = 0; i < size; i++)
        if (open[i].List.size() > 0)
            if (open[i].List.begin()->F <= min.F || min.F == -1) {
                if (open[i].List.begin()->F == min.F && open[i].List.begin()->g > min.g) {
                    min = *open[i].List.begin();
                    exc = i;
                } else if (open[i].List.begin()->F < min.F || min.F == -1) {
                    min = *open[i].List.begin();
                    exc = i;
                }
            }
    if (min.F != -1) {
        element = new TiXmlElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, min.j);
        element->SetAttribute(CNS_TAG_ATTR_Y, min.i);
        element->SetDoubleAttribute(CNS_TAG_ATTR_F, min.F);
        element->SetDoubleAttribute(CNS_TAG_ATTR_G, min.g);
        if (min.g > 0) {
            element->SetAttribute(CNS_TAG_ATTR_PARX, min.parent->j);
            element->SetAttribute(CNS_TAG_ATTR_PARY, min.parent->i);
        }
        child->InsertEndChild(*element);
    }
    for (int i = 0; i < size; i++)
        if (open[i].List.size() > 0)
            for (std::list<Node>::const_iterator it = open[i].List.begin(); it != open[i].List.end(); ++it) {
                if (it != open[exc].List.begin()) {
                    element->Clear();
                    element->SetAttribute(CNS_TAG_ATTR_X, it->j);
                    element->SetAttribute(CNS_TAG_ATTR_Y, it->i);
                    element->SetDoubleAttribute(CNS_TAG_ATTR_F, it->F);
                    element->SetDoubleAttribute(CNS_TAG_ATTR_G, it->g);
                    if (it->g > 0) {
                        element->SetAttribute(CNS_TAG_ATTR_PARX, it->parent->j);
                        element->SetAttribute(CNS_TAG_ATTR_PARY, it->parent->i);
                    }
                    child->InsertEndChild(*element);
                }
            }

    element = new TiXmlElement(CNS_TAG_CLOSE);
    lowlevel->InsertEndChild(*element);
    child = lowlevel->LastChild();

    for (auto it = close.begin(); it != close.end(); ++it) {
        element = new TiXmlElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, it->j);
        element->SetAttribute(CNS_TAG_ATTR_Y, it->i);
        element->SetAttribute(CNS_TAG_ATTR_Z, it->z);
        element->SetDoubleAttribute(CNS_TAG_ATTR_F, it->F);
        element->SetDoubleAttribute(CNS_TAG_ATTR_G, it->g);
        if (it->g > 0) {
            element->SetAttribute(CNS_TAG_ATTR_PARX, it->parent->j);
            element->SetAttribute(CNS_TAG_ATTR_PARY, it->parent->i);
            element->SetAttribute(CNS_TAG_ATTR_PARZ, it->parent->z);
        }
        child->InsertEndChild(*element);
    }
}

void XmlLogger::writeToLogPath(const NodeList &path) {
    if (loglevel == CN_SP_LL_NOLOG || path.List.size() == 0) return;
    int iterate = 0;
    TiXmlElement *element;
    TiXmlNode *lplevel = doc->FirstChild(CNS_TAG_ROOT);
    lplevel = lplevel->FirstChild(CNS_TAG_LOG)->FirstChild(CNS_TAG_LPLEVEL);

    for (std::list<Node>::const_iterator it = path.List.begin(); it != path.List.end(); ++it, ++iterate) {
        element = new TiXmlElement(CNS_TAG_POINT);
        element->SetAttribute(CNS_TAG_ATTR_X, it->j);
        element->SetAttribute(CNS_TAG_ATTR_Y, it->i);
        element->SetAttribute(CNS_TAG_ATTR_Z, it->z);
        element->SetAttribute(CNS_TAG_ATTR_NUM, iterate);
        lplevel->InsertEndChild(*element);
    }
}

void XmlLogger::writeToLogHPpath(const NodeList &hppath) {
    if (loglevel == CN_SP_LL_NOLOG || hppath.List.size() == 0) return;
    int partnumber = 0;
    TiXmlElement *part;
    TiXmlElement *hplevel = doc->FirstChildElement(CNS_TAG_ROOT);
    hplevel = hplevel->FirstChildElement(CNS_TAG_LOG)->FirstChildElement(CNS_TAG_HPLEVEL);
    std::list<Node>::const_iterator iter = hppath.List.begin();
    std::list<Node>::const_iterator it = hppath.List.begin();

    while (iter != --hppath.List.end()) {
        part = new TiXmlElement(CNS_TAG_SECTION);
        part->SetAttribute(CNS_TAG_ATTR_NUM, partnumber);
        part->SetAttribute(CNS_TAG_ATTR_STX, it->j);
        part->SetAttribute(CNS_TAG_ATTR_STY, it->i);
        part->SetAttribute(CNS_TAG_ATTR_STZ, it->z);
        iter++;
        part->SetAttribute(CNS_TAG_ATTR_FINX, iter->j);
        part->SetAttribute(CNS_TAG_ATTR_FINY, iter->i);
        part->SetAttribute(CNS_TAG_ATTR_FINZ, iter->z);
        part->SetDoubleAttribute(CNS_TAG_ATTR_LENGTH, iter->g - it->g);
        hplevel->LinkEndChild(part);
        it++;
        partnumber++;
    }
}

void XmlLogger::writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, float length, double time) {
    if (loglevel == 0) return;

    std::stringstream str;
    str << time;

    TiXmlNode *summary = doc->FirstChild(CNS_TAG_ROOT);
    summary = summary->FirstChild(CNS_TAG_LOG)->FirstChild(CNS_TAG_SUM);
    TiXmlElement *element = summary->ToElement();
    element->SetAttribute(CNS_TAG_ATTR_NUMOFSTEPS, numberofsteps);
    element->SetAttribute(CNS_TAG_ATTR_NODESCREATED, nodescreated);
    element->SetDoubleAttribute(CNS_TAG_ATTR_LENGTH, length);
    element->SetAttribute(CNS_TAG_ATTR_TIME, str.str().c_str());
}

void XmlLogger::writeToLogNotFound() {
    if (loglevel == CN_SP_LL_NOLOG) return;
    TiXmlNode *node = doc->FirstChild(CNS_TAG_ROOT)->FirstChild(CNS_TAG_LOG)->FirstChild(CNS_TAG_PATH);
    node->LinkEndChild(new TiXmlText("Path NOT found!"));
}
