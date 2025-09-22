#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

class Dessert {
public:
	virtual void makeDessert(const std::string&) = 0;
	virtual void deliver(const std::string& str, std::string& time) = 0;
private:

};

class Cake : public Dessert
{
public:
	void cakeFactory() {
		std::cout << "Cake Factory" << std::endl;
	}
	void makeDessert(const std::string& str) {
		std::cout << "Making " << str << " flavoured cake" << std::endl;
	}

	void deliver(const std::string& str, std::string& time1)
	{
		makeDessert(str);
		std::cout << "Deliver from Cake Factory at " << time1 << std::endl;
	}
};

class IceCream : public Dessert
{
public:
	void iceCreamFactory() {
		std::cout << "Ice Cream Factory" << std::endl;
	}
	void makeDessert(const std::string& str) {
		std::cout << "Making " << str << " flavoured ice cream" << std::endl;
	}

	void deliver(const std::string& str, std::string& time2)
	{
		makeDessert(str);
		std::cout << "Deliver from Ice Cream Factory at " << time2 << std::endl;
	}
};

class Factory {
public:
	Factory() {
		dessert["Cake"] = std::make_unique<Cake>();
		dessert["Ice Cream"] = std::make_unique<IceCream>();
	}
	void makeDessert(const std::string& name, std::string& flavour, std::string& time) {
		dessert[name]->deliver(flavour, time);
	}
private: 
	std::unordered_map<std::string, std::unique_ptr<Dessert>> dessert;

};
