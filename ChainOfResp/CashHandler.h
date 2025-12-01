#ifndef CASH_HANDLER_H
#define CASH_HANDLER_H
#include <memory>

class CashHandler {

protected:
	std::shared_ptr<CashHandler> nextHandler;

public:
	virtual ~CashHandler() = default;
	std::shared_ptr<CashHandler> setNext(std::shared_ptr<CashHandler> handler);

	virtual void dispense(int amount) = 0;
};

class HundredDollarHandler : public CashHandler {
public:
	void dispense(int amount) override;
};

class FiftyDollarHandler : public CashHandler {
public:
	void dispense(int amount) override;
};

class TwentyDollarHandler : public CashHandler {
    public:
	void dispense(int amount) override;
};

class TenDollarHandler : public CashHandler {
    public:
	void dispense(int amount) override;
};

#endif 