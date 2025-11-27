#pragma once

#include <string>

/**
 * @brief Logging interface for dependency injection
 * 
 * Enables testability by allowing mock loggers in tests.
 * 
 * Design Pattern: Strategy (for logging strategies)
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    /**
     * @brief Log an informational message
     */
    virtual void log(const std::string& message) = 0;
    
    /**
     * @brief Log an error message
     */
    virtual void error(const std::string& message) = 0;
    
    /**
     * @brief Log a warning message
     */
    virtual void warning(const std::string& message) = 0;
};
