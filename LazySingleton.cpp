#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class Lazy {
private:
    Lazy() {
        std::cout << "Singleton created in thread " << std::this_thread::get_id() << std::endl;
    }
    static std::shared_ptr<Lazy> instance;
    static std::mutex mtx;

public:
    static std::shared_ptr<Lazy> getInstance() {
        std::lock_guard<std::mutex> lock(mtx);
        if (!instance) {
            instance = std::make_shared<Lazy>();
        }
        return instance;
    }
};

std::shared_ptr<Lazy> Lazy::instance = nullptr;
std::mutex Lazy::mtx;

int main() {

    auto worker = []() {
        auto s = Lazy::getInstance();
        std::cout << "Thread " << std::this_thread::get_id() 
                  << " got instance at address: " << s.get() << std::endl;
    };

    std::vector<std::thread> threads;
    const int num_threads = 10;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto s1 = Lazy::getInstance();
    auto s2 = Lazy::getInstance();
    std::cout << "Main thread: address of s1: " << s1.get() << std::endl;
    std::cout << "Main thread: address of s2: " << s2.get() << std::endl;

    return 0;
}
