#ifndef PAYMENT_H
#define PAYMENT_H
#include <iostream>
#include "PlaceOrder.h"

class Payment: public PlaceOrder{
    public:
    void getPayment(){
        std::cout << "Payment is done successfully." << std::endl;
    }
    void orderStatus() override{
        getPayment();
    }
};

#endif // PAYMENT_H