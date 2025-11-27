#pragma once

#include "ILogger.h"
#include <vector>
#include <memory>

/**
 * @brief Composite logger that forwards to multiple loggers
 * 
 * Allows logging to multiple destinations (file, console, etc.)
 * with a single call.
 * 
 * Design Pattern: Composite
 * - Component: ILogger
 * - Composite: CompositeLogger
 * - Leaves: FileLogger, ConsoleLogger
 */
class CompositeLogger : public ILogger {
public:
    CompositeLogger() = default;
    ~CompositeLogger() override = default;
    
    /**
     * @brief Add a logger to the composite
     */
    void addLogger(std::shared_ptr<ILogger> logger);
    
    /**
     * @brief Remove a logger from the composite
     */
    void removeLogger(const std::shared_ptr<ILogger>& logger);
    
    /**
     * @brief Remove all loggers
     */
    void clearLoggers() noexcept;
    
    /**
     * @brief Get number of loggers
     */
    [[nodiscard]] size_t size() const noexcept { return loggers_.size(); }
    
    void log(const std::string& message) override;
    void error(const std::string& message) override;
    void warning(const std::string& message) override;
    
private:
    std::vector<std::shared_ptr<ILogger>> loggers_;
};
