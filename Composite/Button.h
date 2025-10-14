#pragma once
#include "Component.h"

class Button : public Component {
public:
	Button(const std::string& name);
	void draw() const override;
};