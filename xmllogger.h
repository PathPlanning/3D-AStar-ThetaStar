#ifndef XMLLOGGER_H
#define    XMLLOGGER_H

#include <iostream>
#include "tinyxml2.h"
#include "ilogger.h"
#include "map.h"
#include "list.h"
#include <ctime>

class XmlLogger : public ILogger {

public:
    XmlLogger() {}

    virtual ~XmlLogger() {}

    bool getLog(const char *FileName, const std::string *LogParams);

    void saveLog();

    void writeToLogMap(const Map &Map, const NodeList &path);

    void writeToLogOpenClose(const NodeList *open, const std::unordered_set<Node> &close, int size, bool summary);

    void writeToLogPath(const NodeList &path);

    void writeToLogHPpath(const NodeList &hppath);

    void writeToLogNotFound();

    void writeToLogSummary(unsigned int numberofsteps,
                           unsigned int nodescreated,
                           float length,
                           double time);

private:
    std::string LogFileName;
    tinyxml2::XMLDocument doc;
};

#endif

