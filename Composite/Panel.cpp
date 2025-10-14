#include "Panel.h"

Panel::Panel(const std::string& name) : Component(name) {}

Panel::~Panel() {
	for (auto child : children)
		delete child;
}

void Panel::draw() const {
	std::cout << "Drawing Panel: " << name << std::endl;
	for (auto child : children) {
		child->draw();
	}
}

void Panel::add(Component* component) {
	children.push_back(component);
}

void Panel::remove(Component* component) {
	children.erase(std::remove(children.begin(), children.end(), component), children.end());
}

Component* Panel::getChild(int index) {
	if (index >= 0 && index < (int)children.size())
		return children[index];
	return nullptr;
}