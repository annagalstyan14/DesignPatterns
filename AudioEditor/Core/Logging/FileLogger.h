#pragma once
#include "ILogger.h"
#include <fstream>
#include <mutex>

class FileLogger : public ILogger {
public:
    explicit FileLogger(const std::string& filePath);
    ~FileLogger();
    
    void log(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
    
private:
    std::ofstream file_;
    std::mutex mutex_;
    
    void writeLog(const std::string& level, const std::string& message);
};