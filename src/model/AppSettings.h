#pragma once
#include <sstream>
#include <string>

class AppSettings {
    static const std::string DEFAULT_LOGGING_FILENAME;

    std::string loggingFileName;
    std::string currentLog;
    std::ostringstream sstream;
    bool logErrorsToFile = true;
public:
    void load(const std::string& filename);
    void save(const std::string& filename);
};