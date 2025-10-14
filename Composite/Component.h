#pragma once
#include <string>
#include <iostream>

class Component
{
protected:
	std::string name;

public:
	Component(const std::string& name);
	virtual ~Component() = default;

	virtual void draw() const = 0;
	virtual void add(Component* component) {}
	virtual void remove(Component* component) {}
	virtual Component* getChild(int index) { return nullptr; }

	std::string getName() const;

};
