#ifndef AUCTION_H
#define AUCTION_H
#include <string>
#include <vector>
#include <memory>
#include "Observer.h"

class Auction : public Subject {
private:
    std::string item;
    std::string highestBidder;
    double currentBid;
    std::vector<std::shared_ptr<Observer>> observers;

public: 
    Auction(const std::string& item) : item(item), highestBidder(""), currentBid(0.0) {}

    virtual void placeBid(const std::string& bidder, double bidAmount);
    virtual void registerObserver(std::shared_ptr<Observer> observer) override;
    virtual void removeObserver(std::shared_ptr<Observer> observer) override;
    virtual void notifyObservers() override;
};

#endif