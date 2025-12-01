#include "CashHandler.h"
#include <iostream>
#include <memory>

std::shared_ptr<CashHandler> CashHandler::setNext(std::shared_ptr<CashHandler> handler) {
	nextHandler = handler;
	return handler;
}

void HundredDollarHandler::dispense(int amount) {

	if (amount >= 100) {
		int numBills = amount / 100;
		int remainder = amount % 100;
		std::cout << "Dispensing: " << numBills << " x $100 bill(s)" << std::endl;

		if (remainder != 0 && nextHandler) {
			nextHandler->dispense(remainder);
		}
	}
	else if (nextHandler) {
		nextHandler->dispense(amount);
	}
}

void FiftyDollarHandler::dispense(int amount) {

	if (amount >= 50) {
		int numBills = amount / 50;
		int remainder = amount % 50;
		std::cout << "Dispensing: " << numBills << " x $50 bill(s)" << std::endl;

		if (remainder != 0 && nextHandler) {
			nextHandler->dispense(remainder);
		}
	}
	else if (nextHandler) {
		nextHandler->dispense(amount);
	}
}

void TwentyDollarHandler::dispense(int amount) {

	if (amount >= 20) {
		int numBills = amount / 20;
		int remainder = amount % 20;
		std::cout << "Dispensing: " << numBills << " x $20 bill(s)" << std::endl;

		if (remainder != 0 && nextHandler) {
			nextHandler->dispense(remainder);
		}
	}
	else if (nextHandler) {
		nextHandler->dispense(amount);
	}
}

void TenDollarHandler::dispense(int amount) {

	if (amount >= 10) {
		int numBills = amount / 10;
		int remainder = amount % 10;
		std::cout << "Dispensing: " << numBills << " x $10 bill(s)" << std::endl;

		if (remainder != 0 && nextHandler) {
			nextHandler->dispense(remainder);
		}
	}
	else if (nextHandler) {
		nextHandler->dispense(amount);
	}
}