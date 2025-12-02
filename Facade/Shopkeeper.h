#pragma once
#include "Order.h"
#include "Payment.h"
#include "Delivery.h"
#include <iostream>

class Shopkeeper{
    private:
    Order orderSystem;
    Delivery deliverySystem;
    Payment paymentSystem;

    public:
    void placeOrder(){
        std::cout << "Order getting ready..." << std::endl;
        orderSystem.getOrder();
        deliverySystem.getDelivery();
        paymentSystem.getPayment();
        std::cout << "Order process completed!" << std::endl;
    }
};