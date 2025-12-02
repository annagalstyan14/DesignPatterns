#pragma once
#include "State.h"
#include "NoMoneyState.h"
#include "HasMoneyState.h"
#include "DispensingState.h"
#include "NoItemState.h"

class VendingMachine {
public:
    VendingMachine() : state_(new NoMoneyState(this)) {}
    ~VendingMachine() { delete state_; }
    void setState(State* state) { delete state_; state_ = state; }
    void insertCoin(int amount) { state_->insertCoin(amount); }
    void selectItem(int itemCode) { state_->selectItem(itemCode); }
    void dispenseItem() { state_->dispenseItem(); }
    void returnMoney() { state_->returnMoney(); }
private:
    State* state_;
};

// Now VendingMachine is complete, so we can implement methods that use it

inline void NoMoneyState::insertCoin(int amount) {
    vm_->setState(new HasMoneyState(vm_));
    std::cout << "Coin inserted: " << amount << std::endl;
}

inline void HasMoneyState::selectItem(int itemCode) {
    if (item_available) {
        vm_->setState(new DispensingState(vm_));
    } else {
        vm_->setState(new NoItemState(vm_));
    }
}

inline void DispensingState::dispenseItem() {
    std::cout << "Dispensing item..." << std::endl;
    vm_->setState(new NoMoneyState(vm_));
}
