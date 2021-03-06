#ifndef WALRUS_AST_H
#define WALRUS_AST_H

#include <stdbool.h>
#include <stdlib.h>
#include "symbol_table.h"
#include "tokens.h"
#include "types.h"

// macros for doing proper node type casting for us
#define ast_get_child_index(parent, child) (ast_get_child_index)((ASTNode*)parent, (ASTNode*)child)
#define ast_add_child(parent, child) (ast_add_child)((ASTNode*)parent, (ASTNode*)child)
#define ast_remove_child(parent, child_index) (ast_add_child)((ASTNode*)parent, child_index)


/**
 * An enumeration of all kinds of nodes possible in an abstract syntax tree.
 *
 * The first nibble (4 bits) of the values indicate what category the kind
 * belongs to, which is used to determine the struct type it uses.
 */
typedef enum {
    // core kinds with their own struct types
    AST_DECL                    = 0x1,
    AST_REFERENCE               = 0x2,
    AST_OP_EXPR                 = 0x3,

    // generic kinds; these will be of type ASTNode
    AST_BLOCK                   = 0x10,
    AST_IF_STATEMENT            = 0x20,
    AST_ELSE_STATEMENT          = 0x30,
    AST_FOR_STATEMENT           = 0x40,
    AST_BREAK_STATEMENT         = 0x50,
    AST_CONTINUE_STATEMENT      = 0x60,
    AST_RETURN_STATEMENT        = 0x70,
    AST_INT_LITERAL             = 0x80,
    AST_BOOLEAN_LITERAL         = 0x90,
    AST_CHAR_LITERAL            = 0xa0,
    AST_STRING_LITERAL          = 0xb0,

    // decl kinds; these will be of type ASTDecl
    AST_CLASS_DECL              = 0x11,
    AST_FIELD_DECL              = 0x21,
    AST_METHOD_DECL             = 0x31,
    AST_VAR_DECL                = 0x41,
    AST_PARAM_DECL              = 0x51,

    // reference kinds; these will be of type ASTReference
    AST_LOCATION                = 0x12,
    AST_METHOD_CALL             = 0x22,
    AST_CALLOUT                 = 0x32,

    // operation kinds; these will be of type ASTOperation
    AST_UNARY_OP                = 0x13,
    AST_BINARY_OP               = 0x23,
    AST_ASSIGN_OP               = 0x33
} ASTNodeKind;

/**
 * Generic syntax tree node.
 */
typedef struct ASTNode {
    /**
     * The kind of node this node is.
     */
    ASTNodeKind kind;

    /**
     * The source file the node came from.
     */
    char* file;

    /**
     * The line number the node was found in the source file.
     */
    unsigned int line;

    /**
     * The column number the node was found in the source file.
     */
    unsigned int column;

    /**
     * A pointer to any value you like.
     */
    void* value;

    /**
     * The data type of this expression, if relevant.
     */
    DataType type;

    /**
     * The size of the array.
     */
    size_t child_size;

    /**
     * The number of nodes in the array.
     */
    unsigned int child_count;

    /**
     * A pointer to the start of the array.
     */
    struct ASTNode** children;

    /**
     * The parent node of this one, if any.
     */
    struct ASTNode* parent;
} ASTNode;

/**
 * An abstract syntax tree node that also contains declaration information.
 */
typedef struct {
    ASTNode super;

    /**
     * The identifier name of this declaration.
     */
    char* identifier;

    /**
     * Information about the symbol referred.
     *
     * Yes, another kludge.
     */
    SymbolFlags flags;

    /**
     * The length of the declaration, if any.
     */
    unsigned int length;
} ASTDecl;

/**
 * An abstract syntax tree node that contains an operator.
 */
typedef struct {
    ASTNode super;

    /**
     * The data type of this declaration.
     */
    char* operator;
} ASTOperation;

/**
 * An abstract syntax tree node that contains an identifier.
 */
typedef struct {
    ASTNode super;

    /**
     * The variable identifier name.
     */
    char* identifier;
} ASTReference;

/**
 * Creates an abstract syntax tree node.
 *
 * @param  kind The kind of node.
 * @param  file The file name the node was found in.
 * @return      A shiny new abstract syntax tree node.
 *
 * Be careful; the type and size of the allocated node returned varies depending
 * on the kind given. Proper casting is required to avoid memory bound errors.
 * See the comments in the declaration of ASTNodeKind to see what struct types
 * are returned.
 */
void* ast_create_node(ASTNodeKind kind, char* file);

/**
 * Gets the position of a child in a parent node's child list.
 *
 * @param  parent The parent node.
 * @param  child  The child node.
 * @return        The index of the child, or -1 if the child does not belong to
 *                the parent.
 */
int (ast_get_child_index)(ASTNode* parent, ASTNode* child);

/**
 * Adds an abstract syntax tree node to another node as a child.
 *
 * @param  parent The node to add a child to.
 * @param  child  The node add to parent as a child.
 * @return        An error code.
 */
Error (ast_add_child)(ASTNode* parent, ASTNode* child);

/**
 * Removes an abstract syntax tree node from its parent by its child index.
 *
 * This does not destroy the removed node. If you want to destroy the removed
 * node forever, you must explicitly pass it to ast_destroy().
 *
 * @param  parent The parent node.
 * @param  child  The index of the child node to remove.
 * @return        The node removed.
 */
ASTNode* (ast_remove_child)(ASTNode* parent, unsigned int child_index);

/**
 * Pretty-prints an abstract syntax tree to the console.
 *
 * @param  parent The subtree to print.
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
