#include "WhippedCream.h"
#include <iostream>
#include <string>

WhippedCream::WhippedCream(std::unique_ptr<Beverage> bev) 
    : BeverageDecorator(std::move(bev)) {} 

std::string WhippedCream::getDesc() const{
    return beverage->getDesc() + ", WhippedCream";
}

double WhippedCream::getCost() const{
    return beverage->getCost() + 0.75;
}