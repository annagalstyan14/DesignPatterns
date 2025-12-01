#ifndef ATM_H
#define ATM_H
#include <memory>
#include "CashHandler.h"

class ATM {
private:
	std::shared_ptr<CashHandler> hundredHandler;
	std::shared_ptr<CashHandler> fiftyHandler;
	std::shared_ptr<CashHandler> twentyHandler;
	std::shared_ptr<CashHandler> tenHandler;

public:
	ATM();
	void withdraw(int amount);
};


#endif // !ATM_H