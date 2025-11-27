#pragma once

#include "ILogger.h"
#include <fstream>
#include <mutex>
#include <string>

/**
 * @brief File-based logger implementation
 * 
 * Thread-safe logging to file with timestamps.
 * 
 * Design Pattern: Strategy (ConcreteStrategy for logging)
 */
class FileLogger : public ILogger {
public:
    /**
     * @brief Construct logger that writes to specified file
     * 
     * @param filePath Path to log file (created/appended)
     * @throws std::runtime_error if file cannot be opened
     */
    explicit FileLogger(const std::string& filePath);
    ~FileLogger() override;
    
    // Non-copyable, non-movable (owns file handle)
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;
    FileLogger(FileLogger&&) = delete;
    FileLogger& operator=(FileLogger&&) = delete;
    
    void log(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
    
private:
    void writeLog(const std::string& level, const std::string& message);
    
    std::ofstream file_;
    std::mutex mutex_;
};
