#include <iostream>
#include "ATM.h"

ATM::ATM() {
	hundredHandler = std::make_shared<HundredDollarHandler>();
	fiftyHandler = std::make_shared<FiftyDollarHandler>();
	twentyHandler = std::make_shared<TwentyDollarHandler>();
	tenHandler = std::make_shared<TenDollarHandler>();

	hundredHandler->setNext(fiftyHandler)->setNext(twentyHandler)->setNext(tenHandler);
}

void ATM::withdraw(int amount) {
	if (amount <= 0) {
		std::cout << "Invalid amount" << std::endl;
		return;
	}
	if (amount % 10 != 0) {
		std::cout << "Must be a multiples of $10" << std::endl;
		return;
	}

	std::cout << "\nProcessing Withdrawal..." << amount << ":" << std::endl;
	hundredHandler->dispense(amount);
}