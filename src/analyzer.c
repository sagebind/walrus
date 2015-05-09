#include <stdio.h>
#include <string.h>
#include "analyzer.h"
#include "ast.h"
#include "error.h"
#include "symbol_table.h"


/**
 * Analyzes and optimizes an abstract syntax tree.
 */
Error analyzer_analyze(ASTNode* node, SymbolTable* table)
{
    // create global scope
    symbol_table_begin_scope(table);

    // analyze the root
    analyzer_analyze_node(node, table);

    // make sure a main method exists
    SymbolEntry* main = symbol_table_lookup_anywhere(table, "main");
    if (main == NULL || (main->flags & SYMBOL_FUNCTION) == 0) {
        analyzer_error(node, "No main method defined");
    }

    // close global scope
    symbol_table_end_scope(table);
}

/**
 * Displays an analyzer error.
 */
Error analyzer_error(ASTNode* node, char* message)
{
    // display the error message
    return error(
        E_ANALYZE_ERROR,
        "in file \"%s\" near line %d, column %d:\n\t%s",
        node->file,
        node->line,
        node->column,
        message
    );
}

/**
 * Recursively analyzes and optimizes an abstract syntax tree subtree.
 */
Error analyzer_analyze_node(ASTNode* node, SymbolTable* table)
{
    bool new_scope = false;

    // if the node is a declaration of some sort, insert it into the symbol table
    if ((node->kind & 0xF) == AST_DECL) {
        char* symbol = ((ASTDecl*)node)->identifier;

        // make sure the symbol doesn't already exist in the current scope
        if (symbol_table_exists_local(table, symbol)) {
            // symbol already exists
            analyzer_error(node, "Symbol already declared");
        }

        // if a field is an array, validate the length
        if (node->kind == AST_FIELD_DECL && (((ASTDecl*)node)->flags & SYMBOL_ARRAY) == SYMBOL_ARRAY) {
            if (((ASTDecl*)node)->length < 1) {
                analyzer_error(node, "Invalid array size");
            }
        }

        // insert the declaration into the symbol table
        symbol_table_insert(table, symbol, node->type, ((ASTDecl*)node)->flags);
    }

    // the following node kinds open up a new scope level
    if (node->kind == AST_CLASS_DECL || node->kind == AST_METHOD_DECL || node->kind == AST_FOR_STATEMENT || (node->kind == AST_BLOCK && node->parent->kind != AST_METHOD_DECL && node->parent->kind != AST_FOR_STATEMENT)) {
        // kind of a kludge here; open up for and method scopes one level up
        // instead of at the block because AST is weird

        // open up a new scope level
        symbol_table_begin_scope(table);
        new_scope = true;
    }

    // if the node is a unary minus, do fixes if necessary
    if (node->kind == AST_UNARY_OP && ((ASTOperation*)node)->operator[0] == '-') {
        analyzer_fix_minus_int(&node);
    }

    // if the node is some kind of expression, determine its type now
    if ((node->kind & 0xF) == AST_REFERENCE || (node->kind & 0xF) == AST_OP_EXPR || node->kind == AST_RETURN_STATEMENT || node->kind == AST_INT_LITERAL || node->kind == AST_BOOLEAN_LITERAL || node->kind == AST_CHAR_LITERAL || node->kind == AST_STRING_LITERAL) {
        analyzer_determine_expr_type(node, table);
    }

    // make sure arrays are used properly
    if (node->kind == AST_LOCATION) {
        SymbolEntry* entry = symbol_table_lookup(table, ((ASTReference*)node)->identifier);

        // accessing a non-array
        if (node->child_count > 0 && (entry->flags & SYMBOL_ARRAY) != SYMBOL_ARRAY) {
            analyzer_error(node, "Array access on a non-array");
        }

        // array index should be int
        if ((entry->flags & SYMBOL_ARRAY) == SYMBOL_ARRAY) {
            if (node->child_count != 1) {
                analyzer_error(node, "No array index given");
            }

            else if (node->children[0]->type != TYPE_INT) {
                analyzer_error(node, "Array index must be an integer");
            }
        }
    }

    // make sure returns match the types of the functions
    if (node->kind == AST_RETURN_STATEMENT) {
        // find method return statement is in
        ASTNode* method = node->parent;
        while (method->kind != AST_METHOD_DECL) {
            method = method->parent;
        }

        if (node->type != method->type) {
            analyzer_error(node, "Return value type must match method type");
        }
    }

    // analyze each child node
    for (int i = 0; i < node->child_count; ++i) {
        analyzer_analyze_node(node->children[i], table);
    }

    // now that child nodes have been examined, verify if and for statements have
    // proper expression types in them
    if (node->kind == AST_IF_STATEMENT) {
        if (node->children[0]->type != TYPE_BOOLEAN) {
            analyzer_error(node, "Expected boolean expression inside if");
        }
    }

    if (node->kind == AST_FOR_STATEMENT) {
        if (node->children[2]->type != TYPE_INT) {
            analyzer_error(node, "Expected loop condition to be int type");
        }
    }

    // validate method call arguments
    if (node->kind == AST_METHOD_CALL) {
        analyzer_check_method_arguments(node, table);
    }

    // finally, close a scope if we opened one earlier
    if (new_scope) {
        symbol_table_end_scope(table);
    }

    return E_SUCCESS;
}

/**
 * Determines the type of an expression.
 *
 * Walks down the expression until the type can be determined. Walks back up,
 * setting types and checking for errors.
 */
Error analyzer_determine_expr_type(ASTNode* node, SymbolTable* table)
{
    // if type is already determined, stop
    if (node->type != TYPE_NONE) {
        return E_SUCCESS;
    }

    // if node is a reference to something, fetch its type from the symbol table
    if ((node->kind & 0xF) == AST_REFERENCE) {
        char* symbol = ((ASTReference*)node)->identifier;
        SymbolEntry* entry = symbol_table_lookup(table, symbol);

        if (entry == NULL) {
            analyzer_error(node, "Unknown symbol");
        } else {
            // if location is an array, we must have a child expression as the
            // accessor
            if ((entry->flags & SYMBOL_ARRAY) == SYMBOL_ARRAY && node->child_count < 1) {
                analyzer_error(node, "Array element not specified");
            } else {
                node->type = entry->type;
            }
        }
    }

    // determine types for child nodes next
    for (int i = 0; i < node->child_count; ++i) {
        analyzer_determine_expr_type(node->children[i], table);
    }

    // assignments "return" the value that is assigned, so the type is inherited
    if (node->kind == AST_ASSIGN_OP) {
        // make sure types match; that the value assigned matches the variable type
        if (node->children[0]->type != node->children[1]->type) {
            analyzer_error(node, "Assignment value does not match location type");
        }

        // += and -= can only be used on ints
        if (((ASTOperation*)node)->operator[0] != '=') {
            if (node->children[0]->type != TYPE_INT) {
                analyzer_error(node, "Left operand not an int");
            }
            if (node->children[1]->type != TYPE_INT) {
                analyzer_error(node, "Right operand not an int");
            }
        }

        // inherit types
        node->type = node->children[0]->type;
    }

    // unary ops
    if (node->kind == AST_UNARY_OP) {
        ASTOperation* op = (ASTOperation*)node;
        // minus
        if (strcmp(op->operator, "-") == 0) {
            // everything has to be an int
            if (node->children[0]->type != TYPE_INT) {
                analyzer_error(node, "Minus operand not an int");
            }
            node->type = TYPE_INT;
        }

        // not operator
        else {
            // everything has to be bool
            if (node->children[0]->type != TYPE_BOOLEAN) {
                analyzer_error(node, "Not operator must be applied to a boolean");
            }
            node->type = TYPE_BOOLEAN;
        }
    }

    // binary ops
    if (node->kind == AST_BINARY_OP) {
        ASTOperation* op = (ASTOperation*)node;
        // check binary arithmetic operations
        if (strcmp(op->operator, "+") == 0 || strcmp(op->operator, "-") == 0 || strcmp(op->operator, "*") == 0 || strcmp(op->operator, "/") == 0 || strcmp(op->operator, "%") == 0) {
            // everything has to be an int
            if (node->children[0]->type != TYPE_INT) {
                analyzer_error(node, "Left operand not an int");
            }
            if (node->children[1]->type != TYPE_INT) {
                analyzer_error(node, "Right operand not an int");
            }
            node->type = TYPE_INT;
        }

        // check relational comparisons
        else if (strcmp(op->operator, "<") == 0 || strcmp(op->operator, "<=") == 0 || strcmp(op->operator, ">=") == 0 || strcmp(op->operator, ">") == 0) {
            // everything has to be an int
            if (node->children[0]->type != TYPE_INT) {
                analyzer_error(node, "Left operand not an int");
            }
            if (node->children[1]->type != TYPE_INT) {
                analyzer_error(node, "Right operand not an int");
            }
            // result is boolean
            node->type = TYPE_BOOLEAN;
        }

        // conditionals
        else {
            // everything has to be bool
            if (node->children[0]->type != TYPE_BOOLEAN) {
                analyzer_error(node, "Left operand not boolean");
            }
            if (node->children[1]->type != TYPE_BOOLEAN) {
                analyzer_error(node, "Right operand not boolean");
            }
            node->type = TYPE_BOOLEAN;
        }
    }

    // return statement
    if (node->kind == AST_RETURN_STATEMENT) {
        // inherit value returned
        if (node->child_count > 0) {
            node->type = node->children[0]->type;
        } else {
            node->type = TYPE_VOID;
        }
    }

    return E_SUCCESS;
}

/**
 * Checks and verifies a method call's arguments.
 */
Error analyzer_check_method_arguments(ASTNode* node, SymbolTable* table)
{
    // first, we need to find the definition of the method being called
    char* method_name = ((ASTReference*)node)->identifier;

    // find the class node
    ASTNode* class_node = node->parent;
    while (class_node->kind != AST_CLASS_DECL) {
        class_node = class_node->parent;
    }

    // now look for the correct method definition
    ASTNode* method_node = NULL;
    for (int i = 0; i < class_node->child_count; ++i) {
        if (class_node->children[i]->kind == AST_METHOD_DECL) {
            if (strcmp(((ASTDecl*)class_node->children[i])->identifier, method_name) == 0) {
                method_node = class_node->children[i];
            }
        }
    }

    if (method_node == NULL) {
        return analyzer_error(node, "Method not defined");
    }

    // get the parameter count
    unsigned int parameter_count = method_node->child_count - 1;

    // if number of args is not the same as the number of parameters, error
    if (node->child_count != parameter_count) {
        return analyzer_error(node, "Wrong number of method arguments");
    }

    // now verify each parameter type by hand
    for (int i = 0; i < parameter_count; ++i) {
        if (node->children[i]->type != method_node->children[i]->type) {
            analyzer_error(node->children[i], "Parameter type mismatch");
        }
    }

    return E_SUCCESS;
}

/**
 * Converts a unary minus operation on an int literal into a negative int if necessary.
 */
Error analyzer_fix_minus_int(ASTNode** node)
{
    // Node is a unary minus operation. Check if the operand (its only child) is
    // an int literal.
    if ((*node)->children[0]->kind == AST_INT_LITERAL) {
        ASTNode* parent = (*node)->parent;
        ASTNode* int_literal = (*node)->children[0];

        // modify the int literal to be negative (lots of pointer stuff here :( )
        *((int*)            int_literal->value) = 0 - *((int*)int_literal->value);
        //  ^cast to int | pointer to value^          ^ dereference

        // below we get rid of the operator node and replace it with the int literal
        // remove the int from the operator
        ast_remove_child(*node, int_literal);
        // get the position of the operator in the parent's children list
        unsigned int pos = ast_get_child_index(parent, *node);
        // and add the int literal as the child instead
        parent->children[pos] = int_literal;
        // and destroy the operator
        //ast_destroy(node); <- don't destroy because it breaks things
        //                      memory leak? who cares!

        // also note that we update what "node" refers to in the parent function
        // so that things don't blow up
        *node = int_literal;
    }
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
static Error analyzer_write_subtree(FILE* stream, ASTNode* parent, int depth)
{
    for (int i = 0; i < depth; ++i) {
        fprintf(stream, "  ");
    }

    char* tag_name;

    // print out a unique readable string identifying the node kind and attributes
    switch (parent->kind) {
        // generic kinds
        case AST_BLOCK:
            tag_name = "block";
            break;
        case AST_IF_STATEMENT:
            tag_name = "if";
            break;
        case AST_ELSE_STATEMENT:
            tag_name = "else";
            break;
        case AST_FOR_STATEMENT:
            tag_name = "for";
            break;
        case AST_BREAK_STATEMENT:
            tag_name = "break";
            break;
        case AST_CONTINUE_STATEMENT:
            tag_name = "continue";
            break;
        case AST_RETURN_STATEMENT:
            tag_name = "return";
            break;
        // literals
        case AST_INT_LITERAL:
            tag_name = "int";
            break;
        case AST_BOOLEAN_LITERAL:
            tag_name = "bool";
            break;
        case AST_CHAR_LITERAL:
            tag_name = "char";
            break;
        case AST_STRING_LITERAL:
            tag_name = "string";
            break;
        // declaration kinds
        case AST_CLASS_DECL:
            tag_name = "class";
            break;
        case AST_FIELD_DECL:
            tag_name = "field";
            break;
        case AST_METHOD_DECL:
            tag_name = "method";
            break;
        case AST_VAR_DECL:
            tag_name = "var";
            break;
        case AST_PARAM_DECL:
            tag_name = "param";
            break;
        // reference kinds
        case AST_LOCATION:
            tag_name = "location";
            break;
        case AST_METHOD_CALL:
            tag_name = "method_call";
            break;
        case AST_CALLOUT:
            tag_name = "callout";
            break;
        // operator kinds
        case AST_UNARY_OP:
            tag_name = "unary_op";
            break;
        case AST_BINARY_OP:
            tag_name = "binary_op";
            break;
        case AST_ASSIGN_OP:
            tag_name = "assign_op";
            break;
        // unknown
        default:
            tag_name = "unknown";
    }

    fprintf(stream, "<%s", tag_name);

    // write file position
    fprintf(stream, " line=\"%d\" column=\"%d\"", parent->line, parent->column);

    // print out the datatype of the node if it has one
    if (parent->type != TYPE_NONE) {
        fprintf(stream, " type=\"%s\"", data_type_string(parent->type));
    }

    // decl node
    if ((parent->kind & 0xF) == AST_DECL || (parent->kind & 0xF) == AST_REFERENCE) {
        fprintf(stream, " identifier=\"%s\"", ((ASTDecl*)parent)->identifier);
    }

    // op expression node
    else if ((parent->kind & 0xF) == AST_OP_EXPR) {
        fprintf(stream, " operator=\"%s\"", ((ASTOperation*)parent)->operator);
    }

    if (parent->kind == AST_FIELD_DECL) {
        fprintf(stream, " length=\"%d\"", ((ASTDecl*)parent)->length);
    }

    else if (parent->kind == AST_INT_LITERAL) {
        fprintf(stream, " value=\"%d\"", *(int*)parent->value);
    }

    else if (parent->kind == AST_BOOLEAN_LITERAL) {
        fprintf(stream, " value=\"%s\"", (*(bool*)parent->value) ? "true" : "false");
    }

    else if (parent->kind == AST_CHAR_LITERAL || parent->kind == AST_STRING_LITERAL) {
        fprintf(stream, " value=\"%s\"", (char*)parent->value);
    }

    if (parent->child_count > 0) {
        fprintf(stream, ">\n");
    } else {
        fprintf(stream, "/>\n");
        return E_SUCCESS;
    }

    // print every child node recursively
    for (int i = 0; i < parent->child_count; i++) {
        // print the child
        Error e;
        if ((e = analyzer_write_subtree(stream, parent->children[i], depth + 1)) != E_SUCCESS) {
            return e;
        }
    }

    for (int i = 0; i < depth; ++i) {
        fprintf(stream, "  ");
    }
    fprintf(stream, "</%s>\n", tag_name);

    return E_SUCCESS;
}

/**
 * Writes out debugging info to a file.
 */
Error analyzer_write_debug_info(ASTNode* root, SymbolTable* table)
{
    // append .dbg to the filename to write to
    char* filename = malloc(strlen(root->file) + 5);
    strcpy(filename, root->file);
    strncat(filename, ".dbg", 4);

    // open the dbg file for writing
    FILE* stream = fopen(filename, "w");
    if (stream == NULL) {
        return error(E_FILE_NOT_FOUND, "The file '%s' could not be opened.", filename);
    }

    // write debug info as xml
    fprintf(stream, "<?xml version=\"1.0\"?>\n");
    fprintf(stream, "<debug file=\"%s\">\n", root->file);

    // write the contents of the symbol table
    fprintf(stream, "  <symbols>\n");
    int scope_id = 0;
    for (SymbolMap* map = table->sheaf_tail; map != NULL; map = map->previous) {
        for (int i = 0; i < SYMBOL_MAP_SIZE; i++) {
            for (SymbolEntry* entry = map->entries[i]; entry != NULL; entry = entry->next) {
                fprintf(stream, "    <symbol name=\"%s\" scope=\"%d\" type=\"%s\"",
                    entry->symbol,
                    scope_id,
                    data_type_string(entry->type));

                if ((entry->flags & SYMBOL_FUNCTION) == SYMBOL_FUNCTION) {
                    fprintf(stream, " function=\"true\"");
                }

                if ((entry->flags & SYMBOL_ARRAY) == SYMBOL_ARRAY) {
                    fprintf(stream, " array=\"true\"");
                }

                fprintf(stream, "/>\n");
            }
        }

        scope_id++;
    }
    fprintf(stream, "  </symbols>\n");

    // write the ast
    fprintf(stream, "\n  <ast>\n");
    analyzer_write_subtree(stream, root, 2);
    fprintf(stream, "  </ast>\n");

    fprintf(stream, "</debug>\n");

    // close the file
    fclose(stream);

    return E_SUCCESS;
}
