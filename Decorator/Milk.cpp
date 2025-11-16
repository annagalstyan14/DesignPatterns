#include "Milk.h"
#include <iostream>
#include <string>

Milk::Milk(std::unique_ptr<Beverage> bev) 
    : BeverageDecorator(std::move(bev)) {} 

std::string Milk::getDesc() const{
    return beverage->getDesc() + ", Milk";
}

double Milk::getCost() const{
    return beverage->getCost() + 0.50;
}