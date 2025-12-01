#include <memory>
#include <iostream>
#include "Observer.h"   
#include "Auction.h"
#include "Bidders.h"

int main() {
    auto auction = std::make_shared<Auction>("Vintage Car");

    auto bidder1 = std::make_shared<Bidder>("Alice", 5000);
    auto bidder2 = std::make_shared<Bidder>("Bob", 3000);
    auto display = std::make_shared<AuctionDisplay>();

    auction->registerObserver(bidder1);
    auction->registerObserver(bidder2);
    auction->registerObserver(display);

    std::cout << "=== Auction Bidding ===" << std::endl;
    auction->placeBid("Alice", 2000);
    auction->placeBid("Bob", 3500);
    auction->placeBid("Alice", 6000);

    return 0;
}