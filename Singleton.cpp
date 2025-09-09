#include <iostream>
#include <memory>

class Singleton {
private:
	Singleton() {
		std::cout << "Singleton instance created." << std::endl;
	}
	~Singleton() {
		std::cout << "Singleton instance deleted." << std::endl;
	}

	struct Deleter {
		void operator()(Singleton* ptr) const {
			delete ptr;
		}
	};
public:

	static Singleton& getInstance() {
		static std::unique_ptr<Singleton> instance = std::make_unique<Singleton>();
		return *instance;
	};

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
};

int main() {
	Singleton& singleton1 = Singleton::getInstance();
	Singleton& singleton2 = Singleton::getInstance();
	std::cout << "Address of singleton1: " << &singleton1 << std::endl;
	std::cout << "Address of singleton2: " << &singleton2 << std::endl;
}