#pragma once
#include "List.h"
#include <stdexcept>
#include <cstddef>

template <typename T>
class List : public IList<T>{
public:
	List();
	~List();
	void push_back(const T& val);
	void pop_back();
	void push_front(const T& val);
	void pop_front();
	std::size_t size() const;
	void clear();
	const T& front() const;
	const T& back() const;
	bool empty() const noexcept;
public:
	class Node {
	public:
		T val;
		Node* next;

		Node(const T& value, Node* nextNode = nullptr)
			: val(value), next(nextNode) {
		}
	};
private:

	Node* m_head;
	Node* m_tail;
	size_t m_size;
};


#include "List.ipp"