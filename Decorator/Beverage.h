#ifndef BEVERAGE_H
#define BEVERAGE_H
#include <string>

class Beverage{

public:
    virtual ~Beverage() = default;
    virtual std::string getDesc() const =0;
    virtual double getCost() const = 0;

};

#endif