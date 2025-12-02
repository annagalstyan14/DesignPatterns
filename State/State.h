#pragma once

class VendingMachine;

class State {
public:
    virtual ~State() = default;
    virtual void insertCoin(int amount) = 0;
    virtual void selectItem(int itemCode) = 0;
    virtual void dispenseItem() = 0;
    virtual void returnMoney() = 0;
protected:
    VendingMachine* vm_;
};
