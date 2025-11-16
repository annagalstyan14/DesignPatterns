#ifndef MILK_H
#define MILK_H
#include <memory>
#include <string>
#include "BeverageDecorator.h"

class Milk : public BeverageDecorator{
public:
    Milk(std::unique_ptr<Beverage> bev);

    std::string getDesc() const override;
    double getCost() const override;

};
#endif