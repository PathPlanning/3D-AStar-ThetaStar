#ifndef ILOGGER_H
#define	ILOGGER_H

#include <ctime>
#include "map.h"
#include "list.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

class ILogger
{
    public:
        virtual bool getLog(const char* FileName, const std::string* LogParams) = 0;
        virtual void saveLog() = 0;
        virtual void writeToLogMap(const Map& map, const NodeList& path) = 0;
        virtual void writeToLogOpenClose(const NodeList *open, const std::unordered_set<Node>& close, int size, bool summary=false) = 0;
        virtual void writeToLogPath(const NodeList& path) = 0;
        virtual void writeToLogHPpath(const NodeList& path) = 0;
        virtual void writeToLogNotFound() = 0;
        virtual void writeToLogSummary(unsigned int numberofsteps, unsigned int nodescreated, float length, double time) = 0;
        virtual ~ILogger() {}
    public:
        float loglevel;
};

#endif

