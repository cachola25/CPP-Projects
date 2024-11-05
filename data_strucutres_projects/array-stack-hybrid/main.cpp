// Test-bed for testing hybrid stack implementation in C++

#include <iostream>
#include "stack.h"

int main() {
    // print divider line
    std::cout << "=======\n";

    // create stack; push a couple of elements
    Stack s1;
    s1.push(10);
    s1.push(20);

    // print top of stack; then pop and print top again
    std::cout << s1.peek() << std::endl;
    s1.pop();
    std::cout << s1.peek() << std::endl;

    // print stack size; then clear stack
    std::cout << "size: " << s1.size() << std::endl;
    s1.clear();

    // print divider line
    std::cout << "===============\n";

    // push 75 elements onto the stack; then print stack size
    for (int i = 0; i < 75; i++) {
        s1.push(i);
    }
    std::cout << "size: " << s1.size() << std::endl;

    // print the element k-deep in the stack, for selected values of k;
    // this includes some non-existent elements
    for (int k = 2; k < 100; k += 13) {
        int n = s1.nthElementDeep(k);
        if (!(n == INT_MIN)) {
            std::cout << k << " deep is " << n << std::endl;
        }
    }
    std::cout << "===============\n";
    // make a copy of the stack
    Stack s2 = s1; // Use copy constructor
    std::cout << "Copying stack and clearing original stack" <<  std::endl;
    // pop the elements off the stack, printing each
    int count = 1;
    while (!s1.isEmpty()) {
        int n = s1.pop();
        if (count % 10 == 0 || s1.isEmpty()) {
            std::cout << n << std::endl;
        } else {
            std::cout << n << ", ";
        }
        count++;
    }
    
    std::cout << "===============\n";
    std::cout << "Checking if the two stacks are different by checking the sizes of each stack" << std::endl;
    std::cout << "size: " << s1.size() << ", " << s2.size() << std::endl;
    std::cout << "===============\n";

    // print the block size
    std::cout << "block size is " << Stack::blockSize() << std::endl;

    std::cout << "===============\n";
    std::cout << "Checking if the two stacks are different by checking the 17th deep element" << std::endl;
    std::cout << "s1: ";
    s1.nthElementDeep(17);
    std::cout << "s2: " << s2.nthElementDeep(17) << std::endl;

    // empty both stacks
    s1.clear();
    s2.clear();

    return 0;
}