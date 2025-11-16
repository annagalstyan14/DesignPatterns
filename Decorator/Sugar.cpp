#include "Sugar.h"
#include <iostream>
#include <string>

Sugar::Sugar(std::unique_ptr<Beverage> bev) 
    : BeverageDecorator(std::move(bev)) {} 

std::string Sugar::getDesc() const{
    return beverage->getDesc() + ", Sugar";
}

double Sugar::getCost() const{
    return beverage->getCost() + 0.25;
}