#pragma once
#include <cstddef>

template <typename T>
class IStack {
public:
	virtual void push(const T& val) = 0;
	virtual void pop() = 0;
	virtual const T& top() const = 0;
	virtual std::size_t size() const noexcept = 0;
	virtual void clear() = 0;
	virtual bool empty() const noexcept = 0;
};