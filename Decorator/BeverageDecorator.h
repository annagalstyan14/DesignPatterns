#ifndef BEVERAGE_DECORATOR_H
#define BEVERAGE_DECORATOR_H
#include "Beverage.h"
#include <string>
#include <memory>

class BeverageDecorator : public Beverage{

protected:
  std::unique_ptr<Beverage> beverage;

public:
    BeverageDecorator(std::unique_ptr<Beverage> bev);
    virtual ~BeverageDecorator();
    
    std::string getDesc() const override = 0;
    double getCost() const override = 0;

};

#endif