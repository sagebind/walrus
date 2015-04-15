#ifndef WALRUS_AST_H
#define WALRUS_AST_H

#include <stdbool.h>
#include "tokens.h"


// forward declarations
typedef struct ASTNode ASTNode;
typedef struct ASTNodeArray ASTNodeArray;

// node method macros
#define AST_PRINT_FN(name) void name(ASTNode* this, int level)
#define AST_DESTROY_FN(name) void name(ASTNode** this)
#define print_indent(level) for (int i = level; i--; printf("  "))


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
struct ASTNode {
    /**
     * The kind of node this node is.
     */
    ASTNodeKind kind;

    /**
     * Pointer to a function that prints out the current node.
     */
    AST_PRINT_FN((*print));

    /**
     * Pointer to a function that destroys the current node.
     */
    AST_DESTROY_FN((*destroy));
};

/**
 * An abstract syntax tree node that also contains declaration information.
 */
typedef struct {
    // 'inherit' from node type
    ASTNode super;

    /**
     * The identifier name of this declaration.
     */
    char* identifier;

    /**
     * The data type of this declaration.
     */
    DataType type;

    int length;
} ASTDecl;

typedef struct {
    // 'inherit' from node type
    ASTDecl super;

    /**
     * An array of field declarations.
     */
    ASTNodeArray* fields;

    /**
     * An array of method declarations.
     */
    ASTNodeArray* methods;
} ASTClassDecl;

typedef struct {
    // 'inherit' from decl type
    ASTDecl super;

    /**
     * An array of parameters.
     */
    ASTNodeArray* params;

    /**
     * Method block definition.
     */
    ASTNode* block;
} ASTMethodDecl;

typedef struct {
    // 'inherit' from node type
    ASTNode super;

    /**
     * The left operand.
     */
    ASTNode* left;

    /**
     * The right operand.
     */
    ASTNode* right;
} ASTBinaryOp;

typedef struct {
    // 'inherit' from node type
    ASTNode super;

    /**
     * A pointer to some value.
     */
    void* value;
} ASTLiteral;

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
 */
void ast_print(ASTNode* parent);

/**
 * Destroys an abstract syntax tree node and all its children.
 *
 * @param  node The node to destroy.
 * @return      An error code.
 */
//Error ast_destroy(ASTNode** node);


/**
 * Structure for storing a dynamic array of node pointers.
 */
struct ASTNodeArray {
    /**
     * A pointer to the start of the array.
     */
    ASTNode** items;

    /**
     * The number of nodes in the array.
     */
    int length;

    /**
     * The size of the array.
     */
    int size;

    /**
     * Adds a node to the node array.
     */
    void (*add)(ASTNodeArray* array, ASTNode* node);
};

/**
 * Creates a new node array.
 */
ASTNodeArray* ast_node_array_create(void);

/**
 * Adds a node to the node array.
 */
void ast_node_array_add(ASTNodeArray* array, ASTNode* node);

/**
 * Prints the items in a node array.
 *
 * @param array The node array to print.
 */
void ast_node_array_print(ASTNodeArray* array, int level);

/**
 * Destroys a node array.
 */
void ast_node_array_destroy(ASTNodeArray** array);

#endif
