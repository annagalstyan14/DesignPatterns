#include "Logger.h"
#include <iostream>


Logger::Logger() {
    file.open("log.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
    }
}

Logger::~Logger() {
    if (file.is_open()) {
        file.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& message) {
    if (file.is_open()) {
        file << message << std::endl;
    }
    std::cout << message << std::endl; // <-- add this line
}


void Logger::setLogFile(const std::string& filename) {
    if (file.is_open()) {
        file.close();
    }
    file.open(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

