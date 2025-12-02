#ifndef ORDER_H
#define ORDER_H
#include <iostream>
#include "PlaceOrder.h"

class Order: public PlaceOrder{
    public:
    void getOrder(){
        std::cout << "Order is placed successfully." << std::endl;
    }
    void orderStatus() override {
        getOrder();
    }
};

#endif // ORDER_H