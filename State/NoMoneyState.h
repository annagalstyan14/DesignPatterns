#pragma once
#include "State.h"
#include <iostream>

class NoMoneyState : public State {
public:
    NoMoneyState(VendingMachine* vm) { vm_ = vm; }
    void insertCoin(int amount) override;  // implemented in VendingMachine.h
    void selectItem(int itemCode) override {
        std::cout << "Please insert money first." << std::endl;
    }
    void dispenseItem() override {
        std::cout << "Please insert money first." << std::endl;
    }
    void returnMoney() override {
        std::cout << "No money to return." << std::endl;
    }
};
