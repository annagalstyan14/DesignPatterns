#include "Coffee.h"
#include <iostream>
#include <string>

SimpleCoffee::SimpleCoffee() = default;

std::string SimpleCoffee::getDesc() const{
    return "Simple Coffee";
}

double SimpleCoffee::getCost() const{
    return 2.00;
}