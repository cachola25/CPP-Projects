#ifndef STACK_H
#define STACK_H

// If STACK_BLOCK_SIZE is not defined, define it as 20
#ifndef STACK_BLOCK_SIZE
    #define STACK_BLOCK_SIZE 20
#endif
#if STACK_BLOCK_SIZE <= 0
    #undef STACK_BLOCK_SIZE
    #define STACK_BLOCK_SIZE 1
#endif

class Stack {
private:
    // Internal BlockNode class for managing blocks of the stack
    struct BlockNode {
        BlockNode* next;
        int block[STACK_BLOCK_SIZE];

        BlockNode() : next(nullptr) {}
    };

    BlockNode* topBlock;
    int topBlockIndex;

public:
    // Constructor
    Stack();

    // Copy constructor
    Stack(const Stack& other);

    // Assignment operator
    Stack& operator=(const Stack& other);

    // Destructor
    ~Stack();

    // Member functions
    void push(int val);
    bool isEmpty() const;
    int peek() const;
    int pop();
    void clear();
    int size() const;
    int nthElementDeep(int depth) const;
    Stack copy() const;

    // The block size (for debugging)
    static int blockSize() { return STACK_BLOCK_SIZE; }
};

#endif