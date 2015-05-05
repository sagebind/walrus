#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tokens.h"
#include "types.h"


/**
 * Creates an abstract syntax tree node.
 */
void* ast_create_node(ASTNodeKind kind, char* file)
{
    // determine what node size to use
    size_t node_size = sizeof(ASTNode);

    // decl node
    if ((kind & 0xF) == AST_DECL) {
        node_size = sizeof(ASTDecl);
    }

    // location node
    else if ((kind & 0xF) == AST_REFERENCE) {
        node_size = sizeof(ASTReference);
    }

    // op expression node
    else if ((kind & 0xF) == AST_OP_EXPR) {
        node_size = sizeof(ASTOperation);
    }

    // allocate memory for the node
    ASTNode* node = malloc(node_size);

    // allocate and initialize children array
    node->child_size = sizeof(ASTNode*) * 10;
    node->child_count = 0;
    node->children = malloc(node->child_size);

    // set other values to default as well
    node->file = file;
    node->line = 0;
    node->column = 0;
    node->parent = NULL;
    node->value = NULL;
    node->type = TYPE_NONE;
    if ((kind & 0xF) == AST_DECL) {
        ((ASTDecl*)node)->flags = 0;
    }

    // set the node kind
    node->kind = kind;

    return node;
}

/**
 * Gets the position of a child in a parent node's child list.
 */
int (ast_get_child_index)(ASTNode* parent, ASTNode* child)
{
    // make sure pointer isn't null
    if (parent == NULL || child == NULL) {
        return error(E_BAD_POINTER, "Bad pointer");
    }

    for (int i = 0; i < parent->child_count; ++i) {
        if (parent->children[i] == child) {
            return i;
        }
    }

    return -1;
}

/**
 * Adds an abstract syntax tree node to another node as a child.
 */
Error (ast_add_child)(ASTNode* parent, ASTNode* child)
{
    // make sure pointer isn't null
    if (parent == NULL || child == NULL) {
        return error(E_BAD_POINTER, "Bad pointer");
    }

    // reallocate if full
    if (parent->child_count * sizeof(ASTNode*) >= parent->child_size) {
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
 * Removes an abstract syntax tree node from its parent by its child index.
 */
ASTNode* (ast_remove_child)(ASTNode* parent, unsigned int child_index)
{
    // make sure pointer isn't null
    if (parent == NULL) {
        error(E_BAD_POINTER, "Bad pointer");
        return NULL;
    }

    // make sure the given index exists
    if (child_index >= parent->child_count) {
        error(E_INVALID_INDEX, "Invalid child node index");
        return NULL;
    }

    // get the node at the given index
    ASTNode* child = parent->children[child_index];

    // shift all remaining children to the left
    parent->child_count--;
    for (int i = child_index; i < parent->child_count; ++i) {
        parent->children[i] = parent->children[i + 1];
    }

    return child;
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
static Error ast_print_subtree(ASTNode* parent, const char* prefix, bool is_tail)
{
    // print out the current node branch and file position
    printf("%s%s [%d:%d]: ", prefix, is_tail ? "└──" : "├──", parent->line, parent->column);

    // print out a unique readable string identifying the node kind and attributes
    switch (parent->kind) {
        // generic kinds
        case AST_BLOCK:
            printf("block");
            break;
        case AST_IF_STATEMENT:
            printf("if");
            break;
        case AST_ELSE_STATEMENT:
            printf("else");
            break;
        case AST_FOR_STATEMENT:
            printf("for");
            break;
        case AST_BREAK_STATEMENT:
            printf("break");
            break;
        case AST_CONTINUE_STATEMENT:
            printf("continue");
            break;
        case AST_RETURN_STATEMENT:
            printf("return");
            break;

        // literals
        case AST_INT_LITERAL:
            printf("%d", *(int*)parent->value);
            break;
        case AST_BOOLEAN_LITERAL:
            printf("\"%s\"", (*(bool*)parent->value) ? "true" : "false");
            break;
        case AST_CHAR_LITERAL:
            printf("\"%s\"", (char*)parent->value);
            break;
        case AST_STRING_LITERAL:
            printf("\"%s\"", (char*)parent->value);
            break;

        // declaration kinds
        case AST_CLASS_DECL:
            printf("class { identifier: %s }", ((ASTDecl*)parent)->identifier);
            break;
        case AST_FIELD_DECL:
            printf("field { identifier: %s, length: %d }",
                ((ASTDecl*)parent)->identifier,
                ((ASTDecl*)parent)->length);
            break;
        case AST_METHOD_DECL:
            printf("method { identifier: %s }", ((ASTDecl*)parent)->identifier);
            break;
        case AST_VAR_DECL:
            printf("variable { identifier: %s }", ((ASTDecl*)parent)->identifier);
            break;
        case AST_PARAM_DECL:
            printf("parameter { identifier: %s }", ((ASTDecl*)parent)->identifier);
            break;

        // reference kinds
        case AST_LOCATION:
            printf("location { identifier: %s }", ((ASTReference*)parent)->identifier);
            break;
        case AST_METHOD_CALL:
            printf("method call { identifier: %s }",
                ((ASTReference*)parent)->identifier);
            break;
        case AST_CALLOUT:
            printf("library call { name: %s }",
                ((ASTReference*)parent)->identifier);
            break;

        // operator kinds
        case AST_UNARY_OP:
            printf("unary op { operator: %s }", ((ASTOperation*)parent)->operator);
            break;
        case AST_BINARY_OP:
            printf("binary op { operator: %s }", ((ASTOperation*)parent)->operator);
            break;
        case AST_ASSIGN_OP:
            printf("assignment { operator: %s }", ((ASTOperation*)parent)->operator);
            break;

        // unknown
        default:
            printf("unknown node kind %#06x", parent->kind);
    }

    // print out the datatype of the node if it has one
    if (parent->type != TYPE_NONE) {
        printf(" -> %s", data_type_string(parent->type));
    }

    printf("\r\n");

    // print every child node recursively
    for (int i = 0; i < parent->child_count; i++) {
        // append to the prefix
        char* child_prefix = (char*)malloc(strlen(prefix) + 7);
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
    if (node == NULL || *node == NULL) {
        return error(E_BAD_POINTER, "Invalid AST node pointer");
    }

    // destroy child nodes, from left to right
    for (int i = 0; i < (*node)->child_count; i++) {
        // destroy child node
        if (ast_destroy(&(*node)->children[i]) != E_SUCCESS) {
            return E_BAD_POINTER;
        }
    }

    // free the children array
    free((*node)->children);

    // destroy the associated value if it has one
    if ((*node)->value != NULL) {
        free((*node)->value);
    }

    // destroy current node
    free(*node);
    *node = NULL;

    return E_SUCCESS;
}
