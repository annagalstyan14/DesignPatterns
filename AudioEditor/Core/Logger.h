#pragma once
#include <fstream>
#include <string>
#include <mutex>

class Logger {
private:
    static Logger* instance;
    std::ofstream file;
    std::mutex mutex_;
    Logger();
public:
    static Logger& getInstance();
    void log(const std::string& message);
    bool setLogFile(const std::string& filename);
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
