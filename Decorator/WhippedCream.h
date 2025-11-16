#ifndef WhippedCream_H
#define WhippedCream_H
#include <memory>
#include <string>
#include "BeverageDecorator.h"

class WhippedCream : public BeverageDecorator{
public:
    WhippedCream(std::unique_ptr<Beverage> bev);

    std::string getDesc() const override;
    double getCost() const override;

};
#endif