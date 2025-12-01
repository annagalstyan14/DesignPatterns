#include "Command.h"
#include "Player.h"
#include <iostream>

void MoveCommand::execute() {
    player->move(dx, dy);
}

void MoveCommand::undo() {
    player->move(-dx, -dy);
    std::cout << "(Undo move)" << std::endl;
}

void JumpCommand::execute() {
    prevY = player->getY();
    player->jump(10);
}

void JumpCommand::undo() {
    player->setY(prevY);
    std::cout << "(Undo jump) " << player->getName() << " back to height " << prevY << std::endl;
}

void AttackCommand::execute() {
    player->attack(target);
}

void AttackCommand::undo() {
    std::cout << "(Undo attack) " << player->getName() << " stops attacking " << target << std::endl;
}