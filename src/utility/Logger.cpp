#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "utility/Utils.h"

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::instantiate(const AppKey &key, bool logToFile, const std::string& filename) {
    Logger& l = instance();
    l.logToFile = logToFile;
    l.filename = isValidRelativePath(filename) ? filename : Defaults::loggerDefaultPath;
    l.initialized = true;
}

void Logger::flush() {
    if (!logToFile || buffer.tellp() == 0) return;
    namespace fs = std::filesystem;

    fs::path logPath = filename;
    try {
        const auto parent = logPath.parent_path();
        if (!parent.empty())
            fs::create_directories(logPath.parent_path());
    } catch (const fs::filesystem_error& fse) {
        std::cerr << "Failed to create log directory " << fse.what() << std::endl;
        return;
    }
    
    std::ofstream out(filename, std::ios::app);
    if (out.is_open())
        out << buffer.str();
    buffer.str("");
    buffer.clear();
}

void Logger::log(const std::string& message) {
    std::cerr << message << std::endl;

    Logger& l = instance();
    if (!l.initialized ||
        !l.logToFile ||
        l.exceededBufferSize) return;

    using namespace std::chrono;
    time_point now = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_c);
    auto timestamp = std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");

    l.buffer << "[" << timestamp << "] " << message << std::endl;

    if (l.buffer.tellp() <= MAX_BUFFER_SIZE) return;
    l.exceededBufferSize = true;
    l.buffer << "...Further logging omitted due to exceeding maximum buffer size of " << MAX_BUFFER_SIZE << " symbols.";
}