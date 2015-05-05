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

    // begin recursively analyzing the ast
    analyzer_analyze_node(ast, table);

    // destroy the symbol table (even if the analyzer encountered an error)
    symbol_table_destroy(&table);

    // propagate errors
    return error_get_last();
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
    bool new_scope = false;

    /* we need to walk and analyze the abstract syntax tree here and fill up the
       symbol table as we go to find errors in the program */

    // the following node kinds open up a new scope level
    if (node->kind == AST_CLASS_DECL || node->kind == AST_BLOCK) {
        // open up a new scope level
        symbol_table_begin_scope(table);
        new_scope = true;
    }

    // if the node is some kind of expression, determine its type now
    if ((node->kind & 0xF) == AST_REFERENCE || (node->kind & 0xF) == AST_OP_EXPR || node->kind == AST_INT_LITERAL || node->kind == AST_BOOLEAN_LITERAL || node->kind == AST_CHAR_LITERAL || node->kind == AST_STRING_LITERAL) {
        analyzer_determine_expr_type(node, table);
    }

    // if the node is a unary minus, do fixes if necessary
    if (node->kind == AST_UNARY_OP && ((ASTOperation*)node)->operator[0] == '-') {
        analyzer_fix_minus_int(&node);
    }

    // if the node is an assignment operator, check to make sure that the datatype is equal on both sides
    if (node->kind == AST_ASSIGN_OP && ((ASTOperation*)node)->operator[0] == '=') {
        analyzer_assignment_datatype_check(&node);
    }

    // if the node is a declaration of some sort, insert it into the symbol table
    if ((node->kind & 0xF) == AST_DECL && node->kind != AST_PARAM_DECL) {
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

    // finally, close a scope if we opened one earlier
    if (new_scope) {
        symbol_table_end_scope(table);
    }

    return E_SUCCESS;
}

/**
 * Determines the type of an expression.
 *
 * Walks down the expression until the type can be determined. Walks back up,
 * setting types and checking for errors.
 */
Error analyzer_determine_expr_type(ASTNode* node, SymbolTable* table)
{
    // if type is already determined, stop
    if (node->type != TYPE_NONE) {
        return E_SUCCESS;
    }

    // if node is a reference to something, fetch its type from the symbol table
    if ((node->kind & 0xF) == AST_REFERENCE) {
        char* symbol = ((ASTReference*)node)->identifier;
        SymbolEntry* entry = symbol_table_lookup(table, symbol);

        if (entry == NULL) {
            analyzer_error(node, "Unknown symbol");
        } else {
            node->type = entry->type;
        }
    }

    // assignments "return" the value that is assigned, so the type is inherited
    if (node->kind == AST_ASSIGN_OP) {
        // determine operand types
        analyzer_determine_expr_type(node->children[0], table);
        analyzer_determine_expr_type(node->children[1], table);

        // make sure types match; that the value assigned matches the variable type
        if (node->children[0]->type != node->children[1]->type) {
            analyzer_error(node, "Assignment value does not match location type");
        }

        // inherit types
        node->type = node->children[0]->type;
    }

    return E_SUCCESS;
}

/**
 * Checks to make sure the datatype on either side of the assignment operator is equal.
 */
Error analyzer_assignment_datatype_check(ASTNode** node)
{

}

/**
 * Converts a unary minus operation on an int literal into a negative int if necessary.
 */
Error analyzer_fix_minus_int(ASTNode** node)
{
    // Node is a unary minus operation. Check if the operand (its only child) is
    // an int literal.
    if ((*node)->children[0]->kind == AST_INT_LITERAL) {
        ASTNode* parent = (*node)->parent;
        ASTNode* int_literal = (*node)->children[0];

        // modify the int literal to be negative (lots of pointer stuff here :( )
        *((int*)            int_literal->value) = 0 - *((int*)int_literal->value);
        //  ^cast to int | pointer to value^          ^ dereference

        // below we get rid of the operator node and replace it with the int literal
        // remove the int from the operator
        ast_remove_child(*node, int_literal);
        // get the position of the operator in the parent's children list
        unsigned int pos = ast_get_child_index(parent, *node);
        // and add the int literal as the child instead
        parent->children[pos] = int_literal;
        // and destroy the operator
        //ast_destroy(node); <- don't destroy because it breaks things
        //                      memory leak? who cares!

        // also note that we update what "node" refers to in the parent function
        // so that things don't blow up
        *node = int_literal;
    }
}

/**
 * Makes sure if statements and for loops contain a boolean argument
 */
Error analyzer_check_if_boolean(ASTNode* node)
{
    bool contains_boolean = false;
    if (node->kind == AST_IF_STATEMENT || node->kind == AST_FOR_STATEMENT) {
        for (int i = 0; i < node->child_count; ++i) {
            if(node->children[i]->kind == AST_BOOLEAN_LITERAL)
                contains_boolean = true;
        }
        //If it's an if statement without a boolean, return this
        if(!contains_boolean && node->kind == AST_IF_STATEMENT)
            return analyzer_error(node, "If statement does not contain a boolean");
        //If it's a for loop without a boolean, return this
        else if(!contains_boolean && node->kind == AST_FOR_STATEMENT)
            return analyzer_error(node, "For loop does not contain a boolean");
    }

    return E_SUCCESS;
}

/**
 * Makes sure a variable exists when called
 */
Error analyzer_check_if_variable_exists(ASTNode* node) //#2
{
    if(node->kind == AST_REFERENCE) {
        ASTReference*  new_node; //Do I need to do this and the following line? Or can I just yolo cast the same variable somehow?
        new_node = (ASTReference*) node;
        if(node->identifier==NULL)
            return analyzer_error(node, "Variable has not been intialized");
    }
    return E_SUCCESS;
} 