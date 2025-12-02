#ifndef DELIVERY_H
#define DELIVERY_H
#include <iostream>
#include "PlaceOrder.h"

class Delivery: public PlaceOrder{
    public:
    void getDelivery(){
        std::cout << "Delivery is done successfully." << std::endl;
    }

    void orderStatus() override {
        getDelivery();
    }
};

#endif // DELIVERY_H