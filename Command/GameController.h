#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H
#include <stack>
#include <memory>
#include "Command.h"

class GameController {
private:
    std::stack<std::shared_ptr<Command>> history;
    std::stack<std::shared_ptr<Command>> redoStack;

public:
    void execute(std::shared_ptr<Command> cmd) {
        cmd->execute();
        history.push(cmd);
        while (!redoStack.empty()) redoStack.pop();
    }

    void undo() {
        if (history.empty()) {
            std::cout << "Nothing to undo!" << std::endl;
            return;
        }
        auto cmd = history.top();
        history.pop();
        cmd->undo();
        redoStack.push(cmd);
    }

    void redo() {
        if (redoStack.empty()) {
            std::cout << "Nothing to redo!" << std::endl;
            return;
        }
        auto cmd = redoStack.top();
        redoStack.pop();
        cmd->execute();
        history.push(cmd);
    }
};

#endif