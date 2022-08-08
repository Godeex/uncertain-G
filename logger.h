#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>

enum LogLevel {logERROR, logWARNING, logINFO, logDEBUG, logDEBUG1, logDEBUG2};
struct Logger {
    static std::ofstream nullStream;
    static std::ofstream os;
    static LogLevel logLevel;
    static void setLogLevel(LogLevel level) {
        logLevel = level;
    }
    static std::ostream& Get(LogLevel level = logINFO) {
        if (level <= logLevel) {
            return os ? os : std::cout;
        }
        return nullStream;
    }
    static void setFile(std::string filename) {
        os.open(filename);
    }
};

#define logger_info Logger::Get(logINFO)
#define logger_error Logger::Get(logERROR)
#define logger_debug Logger::Get(logDEBUG)

#endif