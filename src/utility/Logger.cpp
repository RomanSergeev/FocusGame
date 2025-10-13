#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Logger.h"

Logger* Logger::instance = nullptr;

void Logger::instantiate(const AppKey &key, bool logToFile, const std::string& filename) {
    if (instance == nullptr) instance = new Logger(logToFile, filename);
}

void Logger::flush() {
    if (!logToFile) return;
    std::ofstream out(filename, std::ios::app);
    if (out.is_open())
        out << buffer.str();
    buffer.str("");
    buffer.clear();
}

void Logger::log(const std::string& message) {
    std::cerr << message << std::endl;

    if (!instance || !instance->logToFile || instance->exceededBufferSize) return;

    using namespace std::chrono;
    time_point now = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_c);
    auto timestamp = std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");

    instance->buffer << "[" << timestamp << "] " << message << std::endl;

    if (instance->buffer.tellp() <= MAX_BUFFER_SIZE) return;
    instance->exceededBufferSize = true;
    instance->buffer << "...Further logging omitted due to exceeding maximum buffer size of " << MAX_BUFFER_SIZE << " symbols.";
}