#pragma once
#include <fstream>
#include <string>

class Logger{
private:
    static Logger* instance;
    std::ofstream file;
    Logger();

public:
    static Logger& getInstance();
    void log(const std::string& message);
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    void setLogFile(const std::string& filename);

};
