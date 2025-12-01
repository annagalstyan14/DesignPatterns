#include "Auction.h"
#include <iostream>
#include <algorithm>

void Auction::placeBid(const std::string& bidder, double bidAmount) {
    if (bidAmount > currentBid) {
        currentBid = bidAmount;
        highestBidder = bidder;
        std::cout << bidder << " placed a bid of $" << bidAmount << " on " << item << std::endl;
        notifyObservers();
    } else {
        std::cout << "Bid of $" << bidAmount << " is too low. Current bid is $" << currentBid << std::endl;
    }
}

void Auction::registerObserver(std::shared_ptr<Observer> observer) {
    observers.push_back(observer);
}

void Auction::removeObserver(std::shared_ptr<Observer> observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Auction::notifyObservers() {
    for (const auto& observer : observers) {
        observer->update(item, currentBid, highestBidder);
    }
}   

