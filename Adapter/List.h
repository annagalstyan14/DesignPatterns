#pragma once
#include <cstddef>

template <typename T>
class IList {
public: 
	virtual void push_back(const T& val) = 0;
	virtual void pop_back() = 0;
	virtual void push_front(const T& val) = 0;
	virtual void pop_front() = 0;
	virtual std::size_t size() const = 0;
	virtual void clear() = 0;
	virtual const T& front() const = 0;
	virtual const T& back() const = 0;
	virtual bool empty() const noexcept = 0;
	virtual ~IList() = default;
};