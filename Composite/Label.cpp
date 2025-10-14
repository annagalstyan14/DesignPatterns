#include "Label.h"

Label::Label(const std::string& name) : Component(name) {}

void Label::draw() const {
	std::cout << "Drawing Label: " << name << std::endl;
}