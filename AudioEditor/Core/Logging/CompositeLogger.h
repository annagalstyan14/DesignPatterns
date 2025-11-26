#pragma once
#include "ILogger.h"
#include <vector>
#include <memory>

class CompositeLogger : public ILogger {
public:
    CompositeLogger() = default;
    ~CompositeLogger() = default;
    
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    void log(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
    
private:
    std::vector<std::shared_ptr<ILogger>> loggers_;
};