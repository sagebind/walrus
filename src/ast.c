#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tokens.h"


/**
 * Creates an abstract syntax tree node.
 */
ASTNode* ast_create_node(ASTNodeKind kind)
{
    // allocate node object
    ASTNode* node = malloc(sizeof(ASTNode));

    // allocate and initialize children array
    node->child_size = sizeof(ASTNode*) * 10;
    node->child_count = 0;
    node->children = malloc(node->child_size);

    // set the node kind
    node->kind = kind;

    return node;
}

/**
 * Creates an abstract syntax tree declaration-type node.
 */
ASTDecl* ast_create_decl(ASTNodeKind kind)
{
    // allocate node object
    ASTDecl* node = malloc(sizeof(ASTDecl));

    // allocate and initialize children array
    ((ASTNode*)node)->child_size = sizeof(ASTNode*) * 10;
    ((ASTNode*)node)->child_count = 0;
    ((ASTNode*)node)->children = malloc(((ASTNode*)node)->child_size);

    // set the node kind
    ((ASTNode*)node)->kind = kind;

    return node;
}

/**
 * Adds an abstract syntax tree node to another node as a child.
 */
Error ast_add_child(ASTNode* parent, ASTNode* child)
{
    // make sure pointer isn't null
    if (parent == NULL || child == NULL) {
        return E_BAD_POINTER;
    }

    // reallocate if full
    if (parent->child_count >= parent->child_size) {
        parent->child_size = parent->child_size << 1;
        parent->children = realloc(parent->children, parent->child_size);
    }

    // add to end of array
    parent->children[parent->child_count++] = child;
    // update parent pointer of child
    child->parent = parent;

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
            printf("class( identifier: %s )", ((ASTDecl*)parent)->identifier);
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

            printf("decl( identifier: %s, type: %s )", ((ASTDecl*)parent)->identifier, type_str);
            break;

        case AST_BLOCK:
            printf("block");
            break;

        default:
            printf("%d", parent->kind);
    }
    printf("\r\n");

    // print every child node recursively
    for (int i = 0; i < parent->child_count; i++) {
        // append to the prefix
        char* child_prefix = (char*)malloc(strlen(prefix) + 5);
        strcpy(child_prefix, prefix);
        strcat(child_prefix, is_tail ? "    " : "│   ");

        // print the child
        Error e;
        if ((e = ast_print_subtree(parent->children[i], child_prefix, i == parent->child_count - 1)) != E_SUCCESS) {
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

    // destroy child nodes, from left to right
    for (int i = 0; i < (*node)->child_count; i++) {
        // destroy child node
        if (ast_destroy(&(*node)->children[i]) != E_SUCCESS) {
            return E_UNKNOWN;
        }
    }

    // free the children array
    free((*node)->children);

    // destroy current node
    free(*node);
    *node = NULL;

    return E_SUCCESS;
}
