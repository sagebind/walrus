#ifndef WALRUS_AST_H
#define WALRUS_AST_H

#include <stdbool.h>
#include "tokens.h"


typedef enum {
    TYPE_BOOLEAN,
    TYPE_INT,
    TYPE_VOID
} DataType;

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
 * Base struct for all abstract syntax tree nodes.
 */
typedef struct ASTNode {
    /**
     * The kind of node this node is.
     */
    ASTNodeKind kind;

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
 * An abstract syntax tree node that contains a list of nodes.
 */
typedef struct {
    // 'inherit' from node type
    ASTNode super;

    /**
     * The first child node of the current node.
     */
    struct ASTNode* head;

    /**
     * The last child node of the current node.
     */
    struct ASTNode* tail;
} ASTList;

/**
 * An abstract syntax tree node that also contains declaration information.
 */
typedef struct {
    // 'inherit' from node type
    ASTNode super;

    /**
     * The identifier name of this declaration.
     */
    char* name;

    /**
     * The data type of this declaration.
     */
    DataType type;
} ASTDecl;

/**
 * Creates an abstract syntax tree node.
 *
 * @param  token The lexical token the node represents.
 * @return       A shiny new abstract syntax tree node.
 */
ASTNode* ast_create(ASTNodeKind kind);

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
