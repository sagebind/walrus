#ifndef WALRUS_ANALYZER_H
#define WALRUS_ANALYZER_H

#include "ast.h"
#include "error.h"


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

#endif
