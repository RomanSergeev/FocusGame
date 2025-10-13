#pragma once
#include <string>
#include <sstream>
#include "Constants.h"

class Logger {
    static Logger* instance;
    static const int MAX_BUFFER_SIZE = 10000;

    std::string filename;
    std::ostringstream buffer;
    bool logToFile;
    bool exceededBufferSize = false;

    Logger(bool logToFile, const std::string& filename) : logToFile(logToFile), filename(filename) {};
    ~Logger() { flush(); };

    void flush();
public:
    static void instantiate(const AppKey& key, bool logToFile = false, const std::string& filename = "");
    static void log(const std::string& message);
    template<typename... Args>
    static void logs(Args&&... args) { // renamed just for clarity and to avoid unintended calls
        // Ensure at least one variadic argument
        // static_assert(sizeof...(Args) >= 1, "At least one variadic argument required");
        std::ostringstream oss;
        ((oss << args), ...);
        log(oss.str());
    }
};