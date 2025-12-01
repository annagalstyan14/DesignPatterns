#include "Player.h"
#include <iostream>

void Player::move(int dx, int dy) {
    x += dx;
    y += dy;
    std::cout << name << " moved to (" << x << ", " << y << ")" << std::endl;
}

void Player::jump(int height) {
    y += height;
    std::cout << name << " jumped! Now at height " << y << std::endl;
}

void Player::attack(const std::string& target) {
    std::cout << name << " attacks " << target << "!" << std::endl;
}