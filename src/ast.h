#ifndef WALRUS_AST_H
#define WALRUS_AST_H

#include <stdbool.h>
#include "tokens.h"


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
 */
typedef enum {
    AST_ASSIGN_STATEMENT,
    AST_BINOP_EXPR,
    AST_BLOCK,
    AST_BOOLEAN_LITERAL,
    AST_STRING_LITERAL,
    AST_BREAK_STATEMENT,
    AST_CALL_EXPR,
    AST_CALLOUT_EXPR,
    AST_CHAR_LITERAL,
    AST_CLASS_DECL,
    AST_CONTINUE_STATEMENT,
    AST_FIELD_DECL,
    AST_FOR_STATEMENT,
    AST_IF_STATEMENT,
    AST_INT_LITERAL,
    AST_INVOKE_STATEMENT,
    AST_METHOD_CALL_EXPR,
    AST_METHOD_DECL,
    AST_PLUS_ASSIGN_STATEMENT,
    AST_RETURN_STATEMENT,
    AST_STATEMENT,
    AST_TYPE,
    AST_IDENTIFIER,
    AST_VAR_DECL
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
 * Creates an abstract syntax tree node.
 *
 * @param  kind The kind of node.
 * @return      A shiny new abstract syntax tree node.
 */
ASTNode* ast_create_node(ASTNodeKind kind);

/**
 * Creates an abstract syntax tree declaration-type node.
 *
 * @param  kind The kind of node.
 * @return      A shiny new abstract syntax tree node.
 */
ASTDecl* ast_create_decl(ASTNodeKind kind);

/**
 * Adds an abstract syntax tree node to another node as a child.
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
