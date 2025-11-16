#include "BeverageDecorator.h"

BeverageDecorator::BeverageDecorator(std::unique_ptr<Beverage> bev) 
    : beverage(std::move(bev)) {}

BeverageDecorator::~BeverageDecorator() = default;