#ifndef WALRUS_AST_H
#define WALRUS_AST_H

#include <stdbool.h>
#include "tokens.h"

// macro for doing proper node type casting for us
#define ast_add_child(parent, child) (ast_add_child)((ASTNode*)parent, (ASTNode*)child)


/**
 * All data types allowed in an AST.
 */
typedef enum {
    TYPE_BOOLEAN,
    TYPE_INT,
    TYPE_VOID
} DataType;

/**
 * An enumeration of all kinds of nodes possible in an abstract syntax tree.
 *
 * The first 4 bits of the values indicate what category the kind belongs to,
 * which is used to determine the struct type it uses.
 */
typedef enum {
    // core kinds with their own struct types
    AST_DECL                    = 0x1,
    AST_ASSIGN_STATEMENT        = 0x2,

    // generic kinds
    AST_BINOP_EXPR              = 0x10,
    AST_BLOCK                   = 0x20,
    AST_BOOLEAN_LITERAL         = 0x40,
    AST_STRING_LITERAL          = 0x80,
    AST_BREAK_STATEMENT         = 0x100,
    AST_CALL_EXPR               = 0x200,
    AST_CALLOUT_EXPR            = 0x400,
    AST_CHAR_LITERAL            = 0x800,
    AST_CONTINUE_STATEMENT      = 0x1000,
    AST_EXPR                    = 0x2000,
    AST_FOR_STATEMENT           = 0x4000,
    AST_IF_STATEMENT            = 0x8000,
    AST_INT_LITERAL             = 0x10000,
    AST_INVOKE_STATEMENT        = 0x20000,
    AST_METHOD_CALL_EXPR        = 0x40000,
    AST_PLUS_ASSIGN_STATEMENT   = 0x80000,
    AST_RETURN_STATEMENT        = 0x100000,
    AST_STATEMENT               = 0x200000,
    AST_TYPE                    = 0x400000,
    AST_IDENTIFIER              = 0x800000,

    // decl kinds
    AST_CLASS_DECL              = 0x11,
    AST_FIELD_DECL              = 0x21,
    AST_METHOD_DECL             = 0x41,
    AST_VAR_DECL                = 0x81
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
     * A pointer to any value you like.
     */
    void* value;

    /**
     * The number of nodes in the array.
     */
    int child_count;

    /**
     * The size of the array.
     */
    int child_size;

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
     * The data type of this declaration.
     */
    DataType type;

    /**
     * The length of the declaration, if any.
     */
    int length;
} ASTDecl;

/**
 * An abstract syntax tree node that contains an identifier and assignment operator.
 */
typedef struct {
    ASTNode super;

    /**
     * The identifier name being assigned to.
     */
    char* identifier;

    /**
     * The data type of this declaration.
     */
    char* operator;
} ASTAssign;

/**
 * Creates an abstract syntax tree node.
 *
 * @param  kind The kind of node.
 * @return      A shiny new abstract syntax tree node.
 *
 * Be careful; the type and size of the allocated node returned varies depending
 * on the kind given. Proper casting is required to avoid memory bound errors.
 */
ASTNode* ast_create_node(ASTNodeKind kind);

/**
 * Adds an abstract syntax tree node to another node as a child.
 *
 * @param  parent The node to add a child to.
 * @param  child  The node add to parent as a child.
 * @return        An error code.
 */
Error (ast_add_child)(ASTNode* parent, ASTNode* child);

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

/**
 * Gets a string name for a data type.
 *
 * @param  type The data type in question.
 * @return      A string naming the given type.
 */
char* data_type_string(DataType type);

#endif
