#pragma once
#include <iostream>
#include <string>
#include <memory>

class Shape {
public:
	virtual void draw() const = 0;

	virtual ~Shape() = default;
};



class ShapeFactory {
private:
	class Circle : public Shape {
	public:
		void draw() const override {
			std::cout << "Drawing Circle" << std::endl;
		}
	};

	class Square : public Shape {
	public:
		void draw() const override {
			std::cout << "Drawing Square" << std::endl;
		}
	};
public:
	static std::unique_ptr<Shape>
		createShape(const std::string& shapeType) {
		if (shapeType.empty()) {
			return nullptr;
		}
		if (shapeType == "Circle") {
			return std::make_unique<Circle>();
		}
		else if (shapeType == "Square") {
			return std::make_unique<Square>();
		}
		throw std::invalid_argument("Unknown shape type: " + shapeType);
	}
};