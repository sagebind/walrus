#ifndef WALRUS_SYMBOL_TABLE_H
#define WALRUS_SYMBOL_TABLE_H

// set the size of the hastable for a symbol map
#define SYMBOL_MAP_SIZE 42


/**
 * Stores information about a single symbol.
 */
typedef struct SymbolEntry {
    /**
     * The symbol name as a string.
     */
    char* symbol;

    /**
     * A bitmask of options.
     */
    unsigned int options;

    /**
     * A pointer to the next entry when used in a map bucket.
     */
    struct SymbolEntry* next;
} SymbolEntry;

/**
 * A hash map of symbol entries.
 */
typedef struct SymbolMap {
    /**
     * A sparse array of symbol table map buckets, where the hash of a symbol
     * corresponds to a single entry bucket.
     */
    SymbolEntry** entries;

    /**
     * A pointer to the previous symbol map when used in a symbol table sheaf.
     */
    struct SymbolMap* previous;
} SymbolMap;

/**
 * Stores a pointer to a symbol map that represents a specific lexical scope.
 */
typedef struct ScopeStackNode {
    /**
     * A pointer to a symbol map in the sheaf.
     */
    SymbolMap* map;

    /**
     * A pointer to the next stack node.
     */
    struct ScopeStackNode* next;

    /**
     * A pointer to the previous stack node.
     */
    struct ScopeStackNode* previous;
} ScopeStackNode;

/**
 * Stores a symbol table.
 */
typedef struct {
    /**
     * A pointer to the first symbol map in the sheaf.
     */
    SymbolMap* sheaf_tail; // grab the list by the tail like a real man!

    /**
     * A pointer to the top of a stack of scope pointers.
     */
    ScopeStackNode* stack_top;
} SymbolTable;

/**
 * Creates a new lexical scope-aware symbol table.
 *
 * @return A pointer to a new symbol table.
 */
SymbolTable* symbol_table_create(void);

/**
 * Begins a new lexical scope in the symbol table.
 *
 * @param  table The symbol table to use.
 * @return       An error code.
 */
Error symbol_table_begin_scope(SymbolTable* table);

/**
 * Closes a previously opened lexical scope.
 *
 * @param  table The symbol table to use.
 * @return       An error code.
 */
Error symbol_table_end_scope(SymbolTable* table);

/**
 * Inserts a symbol into the symbol table.
 *
 * @param  table  The symbol table to insert into.
 * @param  symbol The symbol to insert.
 * @return        An error code.
 */
Error symbol_table_insert(SymbolTable* table, char* symbol);

/**
 * Destroys a symbol table and all of its contents.
 *
 * @param  table The symbol table to destroy.
 * @return       An error code.
 */
Error symbol_table_destroy(SymbolTable** table);

/**
 * Computes the hash of a symbol.
 *
 * @param  symbol The symbol name string.
 * @return        A numerical hash value.
 */
unsigned int symbol_hash(char* symbol);

#endif
