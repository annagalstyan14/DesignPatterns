#pragma once
#include "Component.h"
#include <vector>
#include <algorithm>

class Panel : public Component {
public:
	Panel(const std::string& name);
	~Panel();

	void draw() const override;
	void add(Component* component) override;
	void remove(Component* component) override;
	Component* getChild(int index) override;
private:
	std::vector<Component*> children;
};
