#pragma once
#include "State.h"
#include <iostream>

class HasMoneyState : public State {
public:
    HasMoneyState(VendingMachine* vm) { vm_ = vm; }
    void insertCoin(int amount) override {
        std::cout << "Additional coin inserted: " << amount << std::endl;
    }
    void selectItem(int itemCode) override;  // implemented in VendingMachine.h
    void dispenseItem() override {
        std::cout << "Please select an item first." << std::endl;
    }
    void returnMoney() override {
        std::cout << "Returning money." << std::endl;
    }
private:
    bool item_available = true;
};
