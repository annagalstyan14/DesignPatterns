#pragma once
#include "ILogger.h"
#include <string>

class ConsoleLogger : public ILogger {
public:
    void log(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
};