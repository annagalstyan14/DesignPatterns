#ifndef BIDDERS_H
#define BIDDERS_H
#include <string>
#include <iostream>
#include "Observer.h"

class Bidder : public Observer {
private:
    std::string name;
    double maxBid;
public:
    Bidder(const std::string& name, double maxBid) : name(name), maxBid(maxBid) {}

    void update(const std::string& item, double bid, const std::string& bidder) override {
        if (bid <= maxBid) {
            std::cout << name << " is interested in " << item << " at $" << bid << std::endl;
        } else {
            std::cout << name << " can't afford " << item << " at $" << bid << std::endl;
        }
        
        if (bidder == name) {
            std::cout << "  ^ " << name << ": I'm winning!" << std::endl;
        }
    }
};

class AuctionDisplay : public Observer {
public:
    void update(const std::string& item, double bid, const std::string& bidder) override {
        std::cout << "═══════════════════════════════════" << std::endl;
        std::cout << "  ITEM: " << item << std::endl;
        std::cout << "  BID:  $" << bid << std::endl;
        std::cout << "  BY:   " << bidder << std::endl;
        std::cout << "═══════════════════════════════════" << std::endl;
    }
};

#endif