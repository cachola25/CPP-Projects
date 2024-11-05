// Implementation of a hybrid array/linked-list stack in C++

#include "stack.h"
#include <iostream>
#include <climits>
#include <cstring>

// Constructor
Stack::Stack() : topBlock(nullptr), topBlockIndex(-1) {}

// Copy Constructor
Stack::Stack(const Stack& other) : topBlock(nullptr), topBlockIndex(-1) {
    *this = other;
}

// Assignment Operator to make a deep copy of a stack
Stack& Stack::operator=(const Stack& other) {
    if (this != &other) {
        clear(); // Clear existing stack
        if (other.topBlock) {
            // Copy the other stack
            BlockNode* currOther = other.topBlock;
            BlockNode* prevNew = nullptr;
            // Create a reversed list of blocks
            while (currOther) {
                BlockNode* newNode = new BlockNode();
                std::memcpy(newNode->block, currOther->block, sizeof(int) * STACK_BLOCK_SIZE);
                newNode->next = prevNew;
                prevNew = newNode;
                currOther = currOther->next;
            }
            // Reverse the list to restore original order
            BlockNode* currNew = nullptr;
            while (prevNew) {
                BlockNode* nextNode = prevNew->next;
                prevNew->next = currNew;
                currNew = prevNew;
                prevNew = nextNode;
            }
            topBlock = currNew;
            topBlockIndex = other.topBlockIndex;
        }
    }
    return *this;
}

// Destructor
Stack::~Stack() {
    clear();
}

// Push an element onto the stack
void Stack::push(int val) {
    // If the stack is empty or the current block is full, allocate a new block
    if (!topBlock || topBlockIndex >= STACK_BLOCK_SIZE - 1) {
        BlockNode* newBlock = new BlockNode();
        newBlock->next = topBlock;
        topBlock = newBlock;
        topBlockIndex = -1;
    }
    topBlockIndex++;
    topBlock->block[topBlockIndex] = val;
}

// Check if the stack is empty
bool Stack::isEmpty() const {
    return topBlock == nullptr;
}

// Return the top element of the stack
int Stack::peek() const {
    if (isEmpty()) {
        std::cerr << "Stack is Empty" << std::endl;
        return INT_MIN;
    }
    return topBlock->block[topBlockIndex];
}

// Pop an element off the stack
int Stack::pop() {
    if (isEmpty()) {
        return INT_MIN;
    }
    int toReturn = topBlock->block[topBlockIndex];
    topBlockIndex--;
    // If the current block is empty after popping, delete it
    if (topBlockIndex < 0) {
        BlockNode* temp = topBlock;
        topBlock = topBlock->next;
        delete temp;
        if (topBlock) {
            topBlockIndex = STACK_BLOCK_SIZE - 1;
        } else {
            topBlockIndex = -1;
        }
    }
    return toReturn;
}

// Clear the stack
void Stack::clear() {
    while (topBlock) {
        BlockNode* temp = topBlock;
        topBlock = topBlock->next;
        delete temp;
    }
    topBlockIndex = -1;
}

// Get the number of elements in the stack
int Stack::size() const {
    int length = 0;
    BlockNode* current = topBlock;
    int index = topBlockIndex;
    while (current) {
        if (current == topBlock) {
            length += index + 1;
        } else {
            length += STACK_BLOCK_SIZE;
        }
        current = current->next;
    }
    return length;
}

// Return the value nth deep in the stack
int Stack::nthElementDeep(int depth) const {
    int totalSize = size();
    if (depth >= totalSize || depth < 0) {
        std::cerr << "An element " << depth << " deep does not exist" << std::endl;
        return INT_MIN;
    }

    // Keeps track of when we need to move to the next BlockNode
    // and the index of the Nth element
    int index = this->topBlockIndex;

    // Keep track of how deep into the stack we need to go
    int deep = totalSize - depth;
    BlockNode* curr = this->topBlock;
    while (!isEmpty()) {
        if (index < 0) {
            curr = curr->next;
            index = STACK_BLOCK_SIZE - 1;
        }

        if (totalSize == deep) {break;}
        totalSize--;
        index--;
    }
    return curr->block[index];
}

// Make a fresh copy of the stack
Stack Stack::copy() const {
    return Stack(*this);
}