#pragma once
#include "ICommand.h"
#include "../ILogger.h"
#include <memory>
#include <vector>

class CommandHistory {
public:
    explicit CommandHistory(std::shared_ptr<ILogger> logger);
    void executeCommand(std::shared_ptr<ICommand> command);
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
    void clear();
    
private:
    std::vector<std::shared_ptr<ICommand>> history_;
    std::shared_ptr<ILogger> logger_;
    int currentIndex_ = -1;
};