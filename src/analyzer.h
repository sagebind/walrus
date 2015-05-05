#ifndef WALRUS_ANALYZER_H
#define WALRUS_ANALYZER_H

#include "ast.h"
#include "error.h"
#include "symbol_table.h"


/**
 * Analyzes and optimizes a program's abstract syntax tree and looks for errors.
 *
 * @param  ast The root node of an abstract syntax tree to analyze.
 * @return     An error code.
 */
Error analyzer_analyze(ASTNode* ast);

/**
 * Displays an analyzer error.
 *
 * @param  node    The node that has you want to fight with.
 * @param  message The error message.
 * @return         An error code.
 */
Error analyzer_error(ASTNode* node, char* message);

/**
 * Recursively analyzes and optimizes an abstract syntax tree subtree.
 *
 * @param  node  The root node of an abstract syntax tree to analyze.
 * @param  table The symbol table.
 * @return       An error code.
 */
Error analyzer_analyze_node(ASTNode* node, SymbolTable* table);

/**
 * Converts a unary minus operation on an int literal into a negative int if necessary.
 *
 * @param  node  The root node of the operation.
 * @return       An error code.
 */
Error analyzer_fix_minus_int(ASTNode** node);

/**
 * Analyzes a node and checks to see if ti returns a boolean.
 *
 * @param  node 	The  node of an abstract syntax tree to analyze.
 * @return     		An error code.
 */
Error analyzer_check_if_boolean(ASTNode* node);

#endif
