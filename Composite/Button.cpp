#include "Button.h"

Button::Button(const std::string& name) : Component(name) {}

void Button::draw() const {
	std::cout << "Drawing button: " << name << std::endl;
}