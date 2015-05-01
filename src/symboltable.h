#ifndef WALRUS_SYMBOLTABLE_H
#define WALRUS_SYMBOLTABLE_H


typedef struct SymbolTableNode {
    struct SymbolTableNode* previous;
    HashMap hashmap;
} SymbolTableNode;


typedef struct {
    SymbolTableNode* tail; // grab the linked list by the tail like a real man!
    SymbolTableNode* current;
} SymbolTable;


SymbolTable* symbol_table_create(void);

void symbol_table_new_scope(void);

void insert(char* element);

void lookup(char* inp);

#endif
