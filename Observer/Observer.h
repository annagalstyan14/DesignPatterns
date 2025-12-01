#ifndef OBSERVER_H
#define OBSERVER_H
#include <vector>
#include <memory>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& item, double bid, const std::string& bidder) = 0;
};


class Subject {
    public:
        virtual ~Subject() = default;
        virtual void registerObserver(std::shared_ptr<Observer> observer) = 0;
        virtual void removeObserver(std::shared_ptr<Observer> observer) = 0;
        virtual void notifyObservers() = 0;
    };
#endif