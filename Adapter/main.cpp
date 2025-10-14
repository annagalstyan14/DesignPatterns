#include <iostream>
#include "Adapter.h"

int main() {
    ListStackAdapter<int> stack;

    stack.push(10);
    stack.push(20);
    stack.push(30);

    std::cout << "Top element: " << stack.top() << std::endl;

    stack.pop();

    std::cout << "Top element after pop: " << stack.top() << std::endl;

    return 0;
}