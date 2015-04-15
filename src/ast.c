#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tokens.h"


/**
 * Creates an abstract syntax tree node.
 */
ASTNode* ast_create(ASTNodeKind kind)
{
    ASTNode* node;

    switch (kind) {
        case AST_FIELD_DECL:
        case AST_METHOD_DECL:
        case AST_VAR_DECL:
            node = malloc(sizeof(ASTDecl));
            break;

        default:
            node = malloc(sizeof(ASTNode));
    }

    node->kind = kind;
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
 * @param  parent  The subtree to print.
 * @param  prefix  The string prefix for the current level.
 * @param  is_tail Indicates if the current level is a tail child to the parent.
 * @return         An error code.
 *
 * Here there be dragons, because string manipulation is not fun in C.
 */
static Error ast_print_subtree(ASTNode* parent, char* prefix, bool is_tail)
{
    // print out the current node branch
    printf("%s%s ", prefix, is_tail ? "└──" : "├──");

    // print out a representation of the node data
    char* type_str;
    switch (parent->kind) {
        case AST_CLASS_DECL:
            printf("class");
            break;

        case AST_FIELD_DECL:
        case AST_METHOD_DECL:
        case AST_VAR_DECL:
            if (((ASTDecl*)parent)->type == TYPE_BOOLEAN) {
                type_str = "boolean";
            } else if (((ASTDecl*)parent)->type == TYPE_INT) {
                type_str = "int";
            } else {
                type_str = "void";
            }

            printf("%s %s", type_str, ((ASTDecl*)parent)->name);
            break;

        case AST_BLOCK:
            printf("block");
            break;

        default:
            printf("%d", parent->kind);
    }
    printf("\r\n");

    // print every child node recursively
    for (ASTNode* n = parent->head; n != NULL; n = n->next) {
        // append to the prefix
        char* child_prefix = (char*)malloc(strlen(prefix) + 5);
        strcpy(child_prefix, prefix);
        strcat(child_prefix, is_tail ? "    " : "│   ");

        // print the child
        Error e;
        if ((e = ast_print_subtree(n, child_prefix, n == parent->tail)) != E_SUCCESS) {
            return e;
        }

        // clean up our mess
        free(child_prefix);
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
