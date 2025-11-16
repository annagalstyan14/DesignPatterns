#ifndef SUGAR_H
#define SUGAR_H
#include <memory>
#include <string>
#include "BeverageDecorator.h"

class Sugar : public BeverageDecorator{
public:
    Sugar(std::unique_ptr<Beverage> bev);

    std::string getDesc() const override;
    double getCost() const override;

};
#endif