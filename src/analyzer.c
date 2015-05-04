#include "analyzer.h"
#include "error.h"
#include "symbol_table.h"


/**
 * Analyzes and optimizes a program's abstract syntax tree and looks for errors.
 */
Error analyzer_analyze(ASTNode* ast)
{
    // create a symbol table
    SymbolTable* table = symbol_table_create();
    // create the "global" scope
    symbol_table_begin_scope(table);

    /* we need to walk and analyze the abstract syntax tree here and fill up the
       symbol table as we go to find errors in the program */

    // close the global scope
    symbol_table_end_scope(table);
    // destroy the symbol table
    symbol_table_destroy(&table);
}

/**
 * Displays an analyzer error.
 */
Error analyzer_error(ASTNode* node, char* message)
{
    // display the error message
    return error(
        E_ANALYZE_ERROR,
        "in file \"%s\" near line %d, column %d:\n\t%s",
        node->file,
        node->line,
        node->column,
        message
    );
}

Error analyzer_check_if_boolean(ASTNode* node)
{
    //Need to make sure if() and for() statements contain a boolean value
}
