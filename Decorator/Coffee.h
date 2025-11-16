#ifndef COFFEE_H
#define COFFEE_H
#include <string>
#include "Beverage.h"

class SimpleCoffee : public Beverage{

public: 
    SimpleCoffee();
    std::string getDesc() const override;
    double getCost() const override;

};
#endif