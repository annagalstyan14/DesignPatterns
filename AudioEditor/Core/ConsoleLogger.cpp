// ConsoleLogger.cpp
#include "ConsoleLogger.h"
#include <iostream>
#include <chrono>
#include <iomanip>

static void writeLog(const std::string& level, std::ostream& stream, 
                     const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    stream << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
           << "] [" << level << "] " << message << std::endl;
}

void ConsoleLogger::log(const std::string& message) {
    writeLog("INFO", std::cout, message);
}

void ConsoleLogger::error(const std::string& message) {
    writeLog("ERROR", std::cerr, message);
}

void ConsoleLogger::warning(const std::string& message) {
    writeLog("WARNING", std::cout, message);
}