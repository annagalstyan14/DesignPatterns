#include "Logger.h"
#include <chrono>
#include <iomanip>
#include <iostream>

Logger* Logger::instance = nullptr;

Logger::Logger() {}
Logger::~Logger() { if (file.is_open()) file.close(); }

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::lock_guard<std::mutex> lock(mutex_);
    if (file.is_open()) {
        file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << ": " << message << std::endl;
    }
    std::cout << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << ": " << message << std::endl;
}

bool Logger::setLogFile(const std::string& filename) {
    file.open(filename, std::ios::app);
    return file.is_open();
}