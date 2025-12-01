#include  "ATM.h"
#include <iostream>

int main() {

	ATM atm;
	int Amounts[] = { 280, 150, 90, 45, 174 };

	for (int amount : Amounts) {
		atm.withdraw(amount);
		std::cout << std::endl;
	}
	return 0;
}