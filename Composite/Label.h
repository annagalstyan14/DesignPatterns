#pragma once
#include "Component.h"

class Label : public Component {
public:
	Label(const std::string& name);
	void draw() const override;
};