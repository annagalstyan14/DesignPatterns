#include <memory>
#include <iostream>
#include "Player.h"
#include "Command.h"
#include "GameController.h"

int main() {
    auto player = std::make_shared<Player>("Hero");
    GameController controller;

    std::cout << "=== Executing Commands ===" << std::endl;
    controller.execute(std::make_shared<MoveCommand>(player, 5, 0));
    controller.execute(std::make_shared<MoveCommand>(player, 0, 3));
    controller.execute(std::make_shared<JumpCommand>(player));
    controller.execute(std::make_shared<AttackCommand>(player, "Goblin"));

    std::cout << "\n=== Undo/Redo ===" << std::endl;
    controller.undo();  // Undo attack
    controller.undo();  // Undo jump
    controller.redo();  // Redo jump

    return 0;
}