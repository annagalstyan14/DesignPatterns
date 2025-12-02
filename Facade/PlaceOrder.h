#ifndef PLACEORDER_H
#define PLACEORDER_H
#include <iostream>

class PlaceOrder{
    public:
    virtual void orderStatus() =0;
    virtual ~PlaceOrder() = default;
};

#endif // PLACEORDER_H