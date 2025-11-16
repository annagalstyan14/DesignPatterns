#include "Coffee.h"
#include "Milk.h"
#include "Sugar.h"
#include "WhippedCream.h"
#include <iostream>
#include <memory>

int main() {

    std::unique_ptr<Beverage> coffee = std::make_unique<SimpleCoffee>();

    coffee = std::make_unique<Milk>(std::move(coffee));
    coffee = std::make_unique<Sugar>(std::move(coffee));
    coffee = std::make_unique<WhippedCream>(std::move(coffee));
    
    std::cout << coffee->getDesc() << std::endl;
    std::cout << "$" << coffee->getCost() << std::endl;
    
    
    return 0;
}