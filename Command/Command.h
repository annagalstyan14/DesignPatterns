#ifndef COMMAND_H
#define COMMAND_H
#include <memory>
#include <string>

class Player;

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class MoveCommand : public Command {
private:
    std::shared_ptr<Player> player;
    int dx, dy;
public:
    MoveCommand(std::shared_ptr<Player> p, int dx, int dy)
        : player(p), dx(dx), dy(dy) {}
    
    void execute() override;
    void undo() override;
};

class JumpCommand : public Command {
private:
    std::shared_ptr<Player> player;
    int prevY;
public:
    JumpCommand(std::shared_ptr<Player> p)
        : player(p), prevY(0) {}
    
    void execute() override;
    void undo() override;
};

class AttackCommand : public Command {
private:
    std::shared_ptr<Player> player;
    std::string target;
public:
    AttackCommand(std::shared_ptr<Player> p, const std::string& t)
        : player(p), target(t) {}
    
    void execute() override;
    void undo() override;
};

#endif