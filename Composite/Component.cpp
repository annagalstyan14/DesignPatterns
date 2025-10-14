#include "Component.h"

Component::Component(const std::string& name) : name(name) {}

std::string Component::getName() const { return name; }
