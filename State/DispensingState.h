#pragma once
#include "State.h"
#include <iostream>

class DispensingState : public State {
public:
    DispensingState(VendingMachine* vm) { vm_ = vm; }
    void insertCoin(int amount) override {
        std::cout << "Please wait, dispensing item." << std::endl;
    }
    void selectItem(int itemCode) override {
        std::cout << "Please wait, dispensing item." << std::endl;
    }
    void dispenseItem() override;  // implemented in VendingMachine.h
    void returnMoney() override {
        std::cout << "Cannot return money, dispensing in progress." << std::endl;
    }
};
