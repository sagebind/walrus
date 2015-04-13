#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tokens.h"


/**
 * Creates an abstract syntax tree node.
 */
ASTNode* ast_create(Token token)
{
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->token = token;
    return node;
}

/**
 * Adds an abstract syntax tree node as a child to another node.
 */
Error ast_add_child(ASTNode* parent, ASTNode* child)
{
    // make sure pointer isn't null
    if (parent == NULL || child == NULL) {
        return E_BAD_POINTER;
    }

    // set the parent pointer to the new parent node
    child->parent = parent;

    // add child to parent's list of children
    if (parent->head == NULL) {
        parent->head = parent->tail = child;
        child->previous = child->next = NULL;
    } else {
        parent->tail->next = child;
        child->previous = parent->tail;
        child->next = NULL;
        parent->tail = child;
    }

    return E_SUCCESS;
}

/**
 * Pretty-prints an abstract syntax tree to the console.
 *
 * @param  parent The subtree to print.
 * @param  prefix The string prefix for the current level.
 * @param  isTail Indicates if the current level is a tail child to the parent.
 * @return        An error code.
 *
 * Here there be dragons, because string manipulation is not fun in C.
 */
static Error ast_print_subtree(ASTNode* parent, char* prefix, bool isTail)
{
    // print out the current node
    printf("%s%s %s\r\n", prefix, isTail ? "└──" : "├──", parent->token.lexeme);

    // print every child node recursively
    for (ASTNode* n = parent->head; n != NULL; n = n->next) {
        // append to the prefix
        char* childPrefix = (char*)malloc(strlen(prefix) + 5);
        strcpy(childPrefix, prefix);
        strcat(childPrefix, isTail ? "    " : "│   ");

        // print the child
        Error e;
        if ((e = ast_print_subtree(n, childPrefix, n == parent->tail)) != E_SUCCESS) {
            return e;
        }

        // clean up our mess
        free(childPrefix);
    }

    return E_SUCCESS;
}

/**
 * Pretty-prints an abstract syntax tree to the console.
 */
Error ast_print(ASTNode* parent)
{
    return ast_print_subtree(parent, "", true);
}

/**
 * Destroys an abstract syntax tree node and all its children.
 */
Error ast_destroy(ASTNode** node)
{
    // make sure pointer isn't null
    if (node == NULL) {
        return E_BAD_POINTER;
    }

    // make pointers to the nodes we are currently at
    ASTNode** current = &(*node)->head;
    ASTNode** next;

    // destroy child nodes, from left to right
    while (*current != NULL) {
        // capture the next child node
        next = &(*current)->next;
        // destroy the current child node
        ast_destroy(current);
        // move to the next child node
        current = next;
    }

    // destroy current node
    free(*node);
    *node = NULL;

    return E_SUCCESS;
}
