#include "logger.h"

LogLevel Logger::logLevel = logINFO;
std::ofstream Logger::nullStream;
std::ofstream Logger::os;