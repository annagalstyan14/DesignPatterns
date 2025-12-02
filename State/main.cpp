#include <iostream>

class VendingMachine;
class State;
class NoMoneyState;
class HasMoneyState;
class DispensingState;
class NoItemState;

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


class VendingMachine {
public:
    VendingMachine();
    ~VendingMachine();
    void setState(State* state);
    void insertCoin(int amount) { state_->insertCoin(amount); }
    void selectItem(int itemCode) { state_->selectItem(itemCode); }
    void dispenseItem() { state_->dispenseItem(); }
    void returnMoney() { state_->returnMoney(); }
private:
    State* state_;
};

class NoMoneyState : public State {
public:
    NoMoneyState(VendingMachine* vm) { vm_ = vm; }
    void insertCoin(int amount) override;
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

class HasMoneyState : public State {
public:
    HasMoneyState(VendingMachine* vm) { vm_ = vm; }
    void insertCoin(int amount) override {
        std::cout << "Additional coin inserted: " << amount << std::endl;
    }
    void selectItem(int itemCode) override;
    void dispenseItem() override {
        std::cout << "Please select an item first." << std::endl;
    }
    void returnMoney() override {
        std::cout << "Returning money." << std::endl;
    }
private:
    bool item_available = true;
};

class DispensingState : public State {
public:
    DispensingState(VendingMachine* vm) { vm_ = vm; }
    void insertCoin(int amount) override {
        std::cout << "Please wait, dispensing item." << std::endl;
    }
    void selectItem(int itemCode) override {
        std::cout << "Please wait, dispensing item." << std::endl;
    }
    void dispenseItem() override;
    void returnMoney() override {
        std::cout << "Cannot return money, dispensing in progress." << std::endl;
    }
};

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

VendingMachine::VendingMachine() : state_(new NoMoneyState(this)) {}
VendingMachine::~VendingMachine() { delete state_; }
void VendingMachine::setState(State* state) {
    delete state_;
    state_ = state;
}

void NoMoneyState::insertCoin(int amount) {
    vm_->setState(new HasMoneyState(vm_));
    std::cout << "Coin inserted: " << amount << std::endl;
}

void HasMoneyState::selectItem(int itemCode) {
    if (item_available) {
        vm_->setState(new DispensingState(vm_));
    } else {
        vm_->setState(new NoItemState(vm_));
    }
}

void DispensingState::dispenseItem() {
    std::cout << "Dispensing item..." << std::endl;
    vm_->setState(new NoMoneyState(vm_));
}

int main() {
    VendingMachine vm;
    vm.insertCoin(5);
    vm.selectItem(1);
    vm.dispenseItem();
    vm.returnMoney();
    return 0;
}