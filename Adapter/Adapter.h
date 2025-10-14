#pragma once
#include "Stack.h"
#include "List.h"
#include "Clist.h"

template <typename T>
class ListStackAdapter : public IStack<T> {
public:
    ListStackAdapter() : list(new List<T>()) {}

    ~ListStackAdapter() {
        delete list;
    }

    void push(const T& val) override {
        list->push_front(val);
    }

    void pop() override {
        list->pop_front();
    }

    const T& top() const override {
        return list->front();
    }

    std::size_t size() const noexcept override {
        return list->size();
    }

    void clear() override {
        list->clear();
    }

    bool empty() const noexcept override {
        return list->empty();
    }

private:
    IList<T>* list;
};
