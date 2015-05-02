#ifndef WALRUS_SYMBOLTABLE_H
#define WALRUS_SYMBOLTABLE_H


typedef struct StackNode {
    struct stackNode* previous;
   // HashMap hashmap;
   int content;
} StackNode;


typedef struct {
    StackNode* tail; // grab the linked list by the tail like a real man!
    StackNode* current;
} Stack;


Stack* stack_create(void);

void push(int* inp);

int* pop(void);

void insert(char* element);

void lookup(char* inp);

void initiateScope(void);

void closeScope(void);

#endif
