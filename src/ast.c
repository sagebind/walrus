#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tokens.h"


AST_PRINT_FN(ast_print_decl)
{
    print_indent(level);
    printf("{\r\n");

    print_indent(level + 1);
    printf("identifier: %s\r\n", ((ASTDecl*)this)->identifier);

    // print out a representation of the node data
    char* type_str;
    if (((ASTDecl*)this)->type == TYPE_BOOLEAN) {
        type_str = "boolean";
    } else if (((ASTDecl*)this)->type == TYPE_INT) {
        type_str = "int";
    } else {
        type_str = "void";
    }

    print_indent(level + 1);
    printf("type: %s\r\n", type_str);

    print_indent(level);
    printf("}\r\n");
}

AST_PRINT_FN(ast_print_class_decl)
{
    print_indent(level);
    printf("{\r\n");

    print_indent(level + 1);
    printf("identifier: %s\r\n", ((ASTDecl*)this)->identifier);

    // print out fields
    print_indent(level + 1);
    printf("fields: ");
    ast_node_array_print(((ASTClassDecl*)this)->fields, level + 1);

    // print out methods
    print_indent(level + 1);
    printf("methods: ");
    ast_node_array_print(((ASTClassDecl*)this)->methods, level + 1);

    print_indent(level);
    printf("}\r\n");
}

/**
 * Creates an abstract syntax tree node.
 */
ASTNode* ast_create(ASTNodeKind kind)
{
    ASTNode* node;

    switch (kind) {
        case AST_CLASS_DECL:
            node = malloc(sizeof(ASTClassDecl));
            ((ASTClassDecl*)node)->fields = ast_node_array_create();
            ((ASTClassDecl*)node)->methods = ast_node_array_create();
            node->print = ast_print_class_decl;
            break;

        case AST_METHOD_DECL:
            node = malloc(sizeof(ASTMethodDecl));
            node->print = ast_print_decl;
            break;

        case AST_FIELD_DECL:
        case AST_VAR_DECL:
            node = malloc(sizeof(ASTDecl));
            node->print = ast_print_decl;
            break;

        default:
            node = malloc(sizeof(ASTNode));
    }

    node->kind = kind;
    return node;
}

/**
 * Pretty-prints an abstract syntax tree to the console.
 */
void ast_print(ASTNode* parent)
{
    parent->print(parent, 0);
}

/**
 * Destroys an abstract syntax tree node and all its children.
 */
/*Error ast_destroy(ASTNode** node)
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
}*/


/**
 * Creates a new node array.
 */
ASTNodeArray* ast_node_array_create()
{
    ASTNodeArray* array = malloc(sizeof(ASTNodeArray));
    array->size = sizeof(ASTNode*) * 10;
    array->length = 0;
    array->items = malloc(array->size);
    array->add = ast_node_array_add;
    return array;
}

/**
 * Adds a node to the node array.
 */
void ast_node_array_add(ASTNodeArray* array, ASTNode* node)
{
    // reallocate if full
    if (array->length >= array->size) {
        array->size = array->size * 2;
        array->items = realloc(array->items, array->size);
    }

    // add to end of array
    array->items[array->length++] = node;
}

/**
 * Prints the items in a node array.
 */
void ast_node_array_print(ASTNodeArray* array, int level)
{
    printf("[\r\n");

    // print out each item
    for (int i = 0; i < array->length; i++) {
        array->items[i]->print(array->items[i], level + 1);
    }

    print_indent(level);
    printf("]\r\n");
}

/**
 * Destroys a node array.
 */
void ast_node_array_destroy(ASTNodeArray** array)
{
    free((*array)->items);
    free(*array);
    *array = NULL;
}
