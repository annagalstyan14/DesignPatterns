#pragma once

#include "ILogger.h"

/**
 * @brief Console-based logger implementation
 * 
 * Writes to stdout (info, warning) and stderr (errors).
 * 
 * Design Pattern: Strategy (ConcreteStrategy for logging)
 */
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger() = default;
    ~ConsoleLogger() override = default;
    
    void log(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
};
