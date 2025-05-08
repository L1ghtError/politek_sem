#include <iostream>
#include <Windows.h>

#define STACK_CAPACITY_BYTE 12288 
#define STACK_CAPACITY (STACK_CAPACITY_BYTE / sizeof(char))

typedef struct {
    char* data;
    size_t size;
    size_t capacity;
} STACK;

void CreateStack(STACK* stack) {
    stack->data = (char*)VirtualAlloc(NULL, STACK_CAPACITY_BYTE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!stack->data) {
        std::cerr << "Failed to allocate memory for stack!\n";
        exit(1);
    }
    stack->size = 0;
    stack->capacity = STACK_CAPACITY;
}

bool isEmpty(STACK* stack) {
    return stack->size == 0;
}

bool isFull(STACK* stack) {
    return stack->size == stack->capacity;
}

void push(STACK* stack, char value) {
    if (isFull(stack)) {
        std::cout << "Stack is full!\n";
        return;
    }
    stack->data[stack->size++] = value;
}

char pop(STACK* stack) {
    if (isEmpty(stack)) {
        std::cout << "Stack is empty!\n";
        return '\0';
    }
    char popped = stack->data[--stack->size];
    std::cout << "Popped element: " << popped << "\n";
    return popped;
}

char top(STACK* stack) {
    if (isEmpty(stack)) {
        std::cout << "Stack is empty!\n";
        return '\0';
    }
    return stack->data[stack->size - 1];
}

size_t getSize(STACK* stack) {
    return stack->size;
}

void showStack(STACK* stack) {
    if (isEmpty(stack)) {
        std::cout << "Stack is empty!\n";
        return;
    }
    std::cout << "Stack content (top to bottom): ";
    for (int i = stack->size - 1; i >= 0; --i) {
        std::cout << stack->data[i] << ' ';
    }
    std::cout << '\n';
}

void deleteStack(STACK* stack) {
    if (stack->data) {
        VirtualFree(stack->data, 0, MEM_RELEASE);
    }
}

int main() {
    STACK myStack{};
    CreateStack(&myStack);

    std::cout << (isEmpty(&myStack) ? "Stack is empty.\n" : "Stack is not empty.\n");

    push(&myStack, 'A');
    push(&myStack, 'B');
    push(&myStack, 'C');

    showStack(&myStack);

    std::cout << "Current top of stack: " << top(&myStack) << "\n";

    pop(&myStack); 
    std::cout << "New top of stack: " << top(&myStack) << "\n";

    showStack(&myStack);

    deleteStack(&myStack);

    return 0;
}