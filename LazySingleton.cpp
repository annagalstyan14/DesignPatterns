#include <iostream>
#include <memory>

class Lazy {
private:
	Lazy() {
		std::cout << "Singleton created" << std::endl;
	}
	static std::shared_ptr<Lazy> instance;

public:
	static std::shared_ptr<Lazy> getInstance() {
		if (!instance) {
			instance = std::make_shared<Lazy>([] {return Lazy(); }());
		}
		return instance;
	}

};

std::shared_ptr<Lazy> Lazy::instance = nullptr;

int main() {
	auto s1 = Lazy::getInstance();
	auto s2 = Lazy::getInstance();


	std::cout << "address of s1: " << s1 << std::endl;
	std::cout << "address of s2: " << s2 << std::endl;

	return 0;
}
