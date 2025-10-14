#pragma once
#include "Clist.h"
#include <stdexcept>
#include <cstddef>

template <typename T>
List<T>::List(): m_head(nullptr), m_tail(nullptr), m_size() {}

template <typename T>
List<T>::~List() {
    clear();
}

template <typename T>
void List<T>::push_front(const T& val) {
    Node* new_node = new Node{ val, m_head };
    m_head = new_node;
    if (!m_tail) {
        m_tail = new_node;
    }
    ++m_size;
}

template <typename T>
void List<T>::push_back(const T& val) {
    Node* new_node = new Node{ val, nullptr };
    if (m_tail) {
        m_tail->next = new_node;
    }
    else {
        m_head = new_node;
    }
    m_tail = new_node;
    ++m_size;
}

template <typename T>
void List<T>::pop_front() {
    if (!m_head) throw std::runtime_error("List is empty");
    Node* temp = m_head;
    m_head = m_head->next;
    delete temp;
    if (!m_head) m_tail = nullptr;
    --m_size;
}

template <typename T>
void List<T>::pop_back() {
    if (!m_head) throw std::runtime_error("List is empty");
    if (m_head == m_tail) {
        delete m_head;
        m_head = m_tail = nullptr;
    }
    else {
        Node* current = m_head;
        while (current->next != m_tail) {
            current = current->next;
        }
        delete m_tail;
        m_tail = current;
        m_tail->next = nullptr;
    }
    --m_size;
}

template <typename T>
std::size_t List<T>::size() const {
    return m_size;
}

template <typename T>
void List<T>::clear() {
    while (m_head) {
        Node* temp = m_head;
        m_head = m_head->next;
        delete temp;
    }
    m_tail = nullptr;
    m_size = 0;
}

template <typename T>
const T& List<T>::front() const {
    if (!m_head) throw std::runtime_error("List is empty");
    return m_head->val;
}

template <typename T>
const T& List<T>::back() const {
    if (!m_tail) throw std::runtime_error("List is empty");
    return m_tail->val;
}

template <typename T>
bool List<T>::empty() const noexcept {
    return m_size == 0;
}