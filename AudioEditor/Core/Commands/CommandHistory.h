#pragma once
#include "ICommand.h"
#include <memory>
#include <vector>

class CommandHistory {
public:
    void executeCommand(std::shared_ptr<ICommand> command);
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
    void clear();
    
private:
    std::vector<std::shared_ptr<ICommand>> history_;
    int currentIndex_ = -1;
};