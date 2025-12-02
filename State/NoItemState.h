#pragma once
#include "State.h"
#include <iostream>

class NoItemState : public State {
public:
    NoItemState(VendingMachine* vm) { vm_ = vm; }
    void insertCoin(int amount) override {
        std::cout << "Cannot insert coin, no items available." << std::endl;
    }
    void selectItem(int itemCode) override {
        std::cout << "No items available to select." << std::endl;
    }
    void dispenseItem() override {
        std::cout << "No items available to dispense." << std::endl;
    }
    void returnMoney() override {
        std::cout << "Returning money." << std::endl;
    }
};
