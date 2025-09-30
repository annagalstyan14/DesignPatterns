#include "Factory.h"
#include <stdexcept>

int main() {
	try {
		auto circle = ShapeFactory::createShape("Circle");
		circle->draw();

		auto square = ShapeFactory::createShape("Square");
		square->draw();
	}
	catch (const std::invalid_argument& e) 
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
	return 0;
}
