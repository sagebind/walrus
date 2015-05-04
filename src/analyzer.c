#include "analyzer.h"
#include "ast.h"
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

    // begin recursively analyzing the ast
    if (analyzer_analyze_node(ast, table) != E_SUCCESS) {
        return error_get_last();
    }

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

/**
 * Recursively analyzes and optimizes an abstract syntax tree subtree.
 */
Error analyzer_analyze_node(ASTNode* node, SymbolTable* table)
{
    /* we need to walk and analyze the abstract syntax tree here and fill up the
       symbol table as we go to find errors in the program */

    // if the node is a declaration of some sort, insert it into the symbol table
    if ((node->kind & 0xF) == AST_DECL) {
        char* symbol = ((ASTDecl*)node)->identifier;

        // make sure the symbol doesn't already exist in the current scope
        if (symbol_table_exists_local(table, symbol)) {
            // symbol already exists
            return analyzer_error(node, "Symbol already declared");
        }

        // insert the declaration into the symbol table
        bool is_function = node->kind == AST_METHOD_DECL; // set if it is a function
        symbol_table_insert(table, symbol, node->type, is_function);
    }

    // analyze each child node
    for (int i = 0; i < node->child_count; ++i) {
        analyzer_analyze_node(node->children[i], table);
    }
}

Error analyzer_check_if_boolean(ASTNode* node)
{
    //Need to make sure if() and for() statements contain a boolean value
}
