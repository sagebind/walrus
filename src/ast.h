#ifndef WALRUS_AST_H
#define WALRUS_AST_H

#include <stdbool.h>
#include "tokens.h"


typedef struct ASTNode {
    /**
     * The token that the node represents.
     */
    Token token;

    /**
     * The parent node of the current node, if any.
     */
    struct ASTNode* parent;

    /**
     * The next sibling to the left of the current node.
     */
    struct ASTNode* previous;

    /**
     * The next sibling to the right of the current node.
     */
    struct ASTNode* next;

    /**
     * The first child node of the current node.
     */
    struct ASTNode* head;

    /**
     * The last child node of the current node.
     */
    struct ASTNode* tail;
} ASTNode;

/**
 * Creates an abstract syntax tree node.
 *
 * @param  token The lexical token the node represents.
 * @return       A shiny new abstract syntax tree node.
 */
ASTNode* ast_create(Token token);

/**
 * Adds an abstract syntax tree node as a child to another node.
 *
 * @param  parent The node to add a child to.
 * @param  child  The node add to parent as a child.
 * @return        An error code.
 */
Error ast_add_child(ASTNode* parent, ASTNode* child);

/**
 * Pretty-prints an abstract syntax tree to the console.
 *
 * @param  parent The subtree to print.
 * @return        An error code.
 */
Error ast_print(ASTNode* parent);

/**
 * Destroys an abstract syntax tree node and all its children.
 *
 * @param  node The node to destroy.
 * @return      An error code.
 */
Error ast_destroy(ASTNode** node);

#endif
