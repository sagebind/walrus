#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"


/**
 * Parses the tokens yielded by a given lexer.
 */
ASTNode* parser_parse(Lexer* lexer)
{
    // the source file should contain a single program (duh!)
    ASTDecl* node;
    if (parser_parse_program(lexer, &node) != E_SUCCESS) {
        error(E_PARSE_ERROR, "Failed to parse file \"%s\".", lexer->context->file);
    }

    return (ASTNode*)node;
}

/**
 * Displays a parser error at the current token.
 */
Error parser_error(Lexer* lexer, char* message)
{
    // get the current token
    Token token = lexer->current_node->token;

    // advance to the end of the statement
    while (token.type != T_STATEMENT_END && token.type != T_EOF) {
        token = lexer_next(lexer);
    }

    // display the error message
    return error(
        E_PARSE_ERROR,
        "in file \"%s\" near line %d, column %d:\n\t%s",
        token.file,
        token.line,
        token.column,
        message
    );
}

/**
 * Tries to parse a numerical string and returns its value.
 */
long parser_str_to_long(char* string)
{
    return strtol(string, NULL, 0);
}

/**
 * Creates a copy of a string with single and double quotes stripped from the
 * ends of the string.
 */
char* parser_strip_quotes(const char* string)
{
    size_t new_length = strlen(string) - 2;
    char* new_string = malloc(sizeof(char) * (new_length + 1));
    strncpy(new_string, string + 1, new_length);
    new_string[new_length] = 0;
    return new_string;
}

/**
 * Checks if a token is a binary operator.
 */
static inline bool token_is_bin_op(Token token)
{
    return token.type == T_DIVIDE
        || token.type == T_IS_EQUAL
        || token.type == T_IS_GREATER
        || token.type == T_IS_GREATER_OR_EQUAL
        || token.type == T_IS_LESSER
        || token.type == T_IS_LESSER_OR_EQUAL
        || token.type == T_IS_NOT_EQUAL
        || token.type == T_LOGICAL_AND
        || token.type == T_LOGICAL_OR
        || token.type == T_MINUS
        || token.type == T_MODULO
        || token.type == T_MULTIPLY
        || token.type == T_PLUS;
}

/**
 * <program> -> class Program { <field_decl_list> <method_decl_list> }
 */
Error parser_parse_program(Lexer* lexer, ASTDecl** node)
{
    *node = ast_create_node(AST_CLASS_DECL, lexer->context->file);
    (*node)->identifier = "Program";

    Token token = lexer_next(lexer);
    if (token.type != T_CLASS) {
        return parser_error(lexer, "A program starts with 'class', you fool.");
    }

    // set line and column
    ((ASTNode*)*node)->line = token.line;
    ((ASTNode*)*node)->column = token.column;

    token = lexer_next(lexer);
    if (token.type != T_IDENTIFIER || strcmp(token.lexeme, "Program") != 0) {
        return parser_error(lexer, "Expecting 'Program'.");
    }

    token = lexer_next(lexer);
    if (token.type != T_BRACE_LEFT) {
        return parser_error(lexer, "Missing opening curly brace for class 'Program'.");
    }

    // both can be empty, so no errors unique to here
    parser_parse_field_decl_list(lexer, *node);
    parser_parse_method_decl_list(lexer, *node);

    token = lexer_next(lexer);
    if (token.type != T_BRACE_RIGHT) {
        return parser_error(lexer, "Missing closing curly brace for class 'Program'.");
    }

    if (lexer_next(lexer).type != T_EOF) {
        return parser_error(lexer, "Expected end of file.");
    }

    return E_SUCCESS;
}

/**
 * <field_decl_list> -> <field_decl> <field_decl_list> | EPSILON
 */
Error parser_parse_field_decl_list(Lexer* lexer, ASTDecl* program)
{
    // do a lookahead
    Token token = lexer_lookahead(lexer, 1);

    // try to parse a field decl?
    if (token.type == T_BOOLEAN || token.type == T_INT) {
        // might be a field decl, do a further lookahead
        token = lexer_lookahead(lexer, 3);

        // start parsing fields
        if (token.type != T_PAREN_LEFT) {
            if (parser_parse_field_decl(lexer, program) != E_SUCCESS) {
                return E_PARSE_ERROR;
            }

            // if that worked, we must have a field_decl_list next
            return parser_parse_field_decl_list(lexer, program);
        }
    }

    // empty string works
    return E_SUCCESS;
}

/**
 * <method_decl_list> -> <method_decl> <method_decl_list> | EPSILON
 */
Error parser_parse_method_decl_list(Lexer* lexer, ASTDecl* program)
{
    Token first_token = lexer_lookahead(lexer, 1);

    // epsilon
    if (first_token.type == T_BRACE_RIGHT) {
        return E_SUCCESS;
    }

    // try to parse a method decl
    ASTDecl* method_decl;
    if (parser_parse_method_decl(lexer, &method_decl) != E_SUCCESS) {
        return E_PARSE_ERROR;
    } else {
        ast_add_child(program, method_decl);
    }

   // if that worked, we must have a method_decl_list
    return parser_parse_method_decl_list(lexer, program);
}

/**
 * <field_decl> -> <type> <field_id_list>
 */
Error parser_parse_field_decl(Lexer* lexer, ASTDecl* program)
{
    // determine the type of the fields listed here
    DataType type;
    if (parser_parse_type(lexer, &type) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }

    // parse each field identifier using the given type
    return parser_parse_field_id_list(lexer, type, program);
}

/**
 * <field_id_list> -> <id> <array_dim_decl> <field_id_list_tail>
 */
Error parser_parse_field_id_list(Lexer* lexer, DataType type, ASTDecl* program)
{
    ASTDecl* node = ast_create_node(AST_FIELD_DECL, lexer->context->file);
    ((ASTNode*)node)->type = type;

    // set line and column
    Token next_token = lexer_lookahead(lexer, 1);
    ((ASTNode*)node)->line = next_token.line;
    ((ASTNode*)node)->column = next_token.column;

    if (parser_parse_id(lexer, &node->identifier) != E_SUCCESS) {
        return parser_error(lexer, "Expected field name.");
    }

    if (parser_parse_array_dim_decl(lexer, &node->length) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }

    if (parser_parse_field_id_list_tail(lexer, type, program) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }

    ast_add_child(program, node);
    return E_SUCCESS;
}

/**
 * <array_dim_decl> -> [ <int_literal> ] | EPSILON
 */
Error parser_parse_array_dim_decl(Lexer* lexer, int* length)
{
    Token token = lexer_lookahead(lexer, 1);

    if (token.type == T_BRACKET_LEFT) {
        lexer_next(lexer);

        ASTNode* int_literal;
        if (parser_parse_int_literal(lexer, &int_literal) != E_SUCCESS) {
            return parser_error(lexer, "Expected array length.");
        }
        // fetch the int value as the length
        *length = *(int*)(int_literal->value);

        token = lexer_next(lexer);
        if (token.type != T_BRACKET_RIGHT) {
            return parser_error(lexer, "Missing closing bracket in array declaration.");
        }
    }

    // empty string works
    return E_SUCCESS;
}

/**
 * <field_id_list_tail> -> , <field_id_list> | ;
 */
Error parser_parse_field_id_list_tail(Lexer* lexer, DataType type, ASTDecl* program)
{
    Token token = lexer_next(lexer);
    if (token.type == T_COMMA) {
        //first derivation
        if (parser_parse_field_id_list(lexer, type, program) != E_SUCCESS) {
            return parser_error(lexer, "Expected field id list.");
        }
    } else if (token.type != T_STATEMENT_END) {
        return parser_error(lexer, "Missing semicolon ';' or comma ',' after field declaration.");
    }

    return E_SUCCESS;
}

/**
 * <method_decl> -> <type> <id> ( <method_param_decl_list> ) <block>
                  | void <id> ( <method_param_decl_list> ) <block>
 */
Error parser_parse_method_decl(Lexer* lexer, ASTDecl** node)
{
    *node = ast_create_node(AST_METHOD_DECL, lexer->context->file);
    ((ASTNode*)*node)->type = TYPE_VOID;

    // set line and column
    Token next_token = lexer_lookahead(lexer, 1);
    ((ASTNode*)*node)->line = next_token.line;
    ((ASTNode*)*node)->column = next_token.column;

    // can start with <type> or void
    if (next_token.type == T_VOID) {
        lexer_next(lexer);
    } else if (parser_parse_type(lexer, &((ASTNode*)*node)->type) != E_SUCCESS) {
        return parser_error(lexer, "Expected type name or void.");
    }

    // must have an identifier next
    if (parser_parse_id(lexer, &((ASTDecl*)*node)->identifier) != E_SUCCESS) {
        return parser_error(lexer, "Expected identifier.");
    }

    // left paren next ...
    if (lexer_next(lexer).type != T_PAREN_LEFT) {
        return parser_error(lexer, "Expected left parentheses when parsing method_decl and did not get one.");
    }

    // ... then the params ...
    if (parser_parse_method_param_decl_list(lexer, *node) != E_SUCCESS) {
        return parser_error(lexer, "Expected method argument list.");
    }

    // ... right paren next ...
    if (lexer_next(lexer).type != T_PAREN_RIGHT) {
        return parser_error(lexer, "Expected right parentheses when parsing method_decl and did not get one.");
    }

    // ... then finally a block
    ASTNode* block;
    if (parser_parse_block(lexer, &block) != E_SUCCESS) {
        return E_PARSE_ERROR;
    } else {
        ast_add_child(*node, block);
    }

    // we made it!
    return E_SUCCESS;
}

/**
 * <method_param_decl_list> -> <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
Error parser_parse_method_param_decl_list(Lexer* lexer, ASTDecl* method)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_PAREN_RIGHT) {
        return E_SUCCESS;
    }

    ASTDecl* param;
    if (parser_parse_method_param_decl(lexer, &param) != E_SUCCESS) {
        return parser_error(lexer, "Expected parameter declaration.");
    } else {
        ast_add_child(method, param);
    }

    if (parser_parse_method_param_decl_list_tail(lexer, method) != E_SUCCESS) {
        parser_error(lexer, "Expected parameter list tail.");
    }

    return E_SUCCESS;
}

/**
 * <method_param_decl_list_tail> -> , <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
Error parser_parse_method_param_decl_list_tail(Lexer* lexer, ASTDecl* method)
{
    // first derivation
    if (lexer_lookahead(lexer, 1).type == T_COMMA) {
        lexer_next(lexer);

        ASTDecl* param;
        if (parser_parse_method_param_decl(lexer, &param) != E_SUCCESS) {
            return parser_error(lexer, "Expected method parameter declaration.");
        } else {
            ast_add_child(method, param);
        }

        if (parser_parse_method_param_decl_list_tail(lexer, method) != E_SUCCESS) {
            return parser_error(lexer, "Expected parameter list tail.");
        }
    }

    // epsilon
    return E_SUCCESS;
}

/**
 * <method_param_decl> -> <type> <id>
 */
Error parser_parse_method_param_decl(Lexer* lexer, ASTDecl** node)
{
    *node = ast_create_node(AST_PARAM_DECL, lexer->context->file);

    // set line and column
    Token next_token = lexer_lookahead(lexer, 1);
    ((ASTNode*)*node)->line = next_token.line;
    ((ASTNode*)*node)->column = next_token.column;

    if (parser_parse_type(lexer, &((ASTNode*)*node)->type) != E_SUCCESS) {
        return parser_error(lexer, "Expected parameter type.");
    }

    if (parser_parse_id(lexer, &(*node)->identifier) != E_SUCCESS) {
        return parser_error(lexer, "Expected parameter identifier.");
    }

    return E_SUCCESS;
}

/**
 * <block> -> { <var_decl_list> <statement_list> }
 */
Error parser_parse_block(Lexer* lexer, ASTNode** node)
{
    *node = ast_create_node(AST_BLOCK, lexer->context->file);

    Token token = lexer_next(lexer);
    if (token.type != T_BRACE_LEFT) {
        return parser_error(lexer, "Missing left curly brace when parsing block.");
    }

    // set line and column
    (*node)->line = token.line;
    (*node)->column = token.column;

    if (parser_parse_var_decl_list(lexer, *node) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }

    if (parser_parse_statement_list(lexer, *node) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }

    if (lexer_next(lexer).type != T_BRACE_RIGHT) {
        return parser_error(lexer, "Missing right curly brace when parsing block.");
    }

    return E_SUCCESS;
}

/**
 * <var_decl_list> -> <var_decl> <var_decl_list> | EPSILON
 */
Error parser_parse_var_decl_list(Lexer* lexer, ASTNode* parent)
{
    Token token = lexer_lookahead(lexer, 1);

    // first derivation
    if (token.type == T_BOOLEAN || token.type == T_INT) {
        if (parser_parse_var_decl(lexer, parent) != E_SUCCESS) {
            return E_PARSE_ERROR;
        }

        if (parser_parse_var_decl_list(lexer, parent) != E_SUCCESS) {
            return E_PARSE_ERROR;
        }
    }

    // epsilon
    return E_SUCCESS;
}

/**
 * <statement_list> -> <statement> <statement_list> | EPSILON
 */
Error parser_parse_statement_list(Lexer* lexer, ASTNode* parent)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_BRACE_RIGHT) {
        return E_SUCCESS;
    }

    // first derivation
    ASTNode* statement;
    if (parser_parse_statement(lexer, &statement) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }
    ast_add_child(parent, statement);

    if (parser_parse_statement_list(lexer, parent) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }

    return E_SUCCESS;
}

/**
 * <var_decl> -> <type> <id> <var_id_list_tail>
 */
Error parser_parse_var_decl(Lexer* lexer, ASTNode* parent)
{
    ASTDecl* node = ast_create_node(AST_VAR_DECL, lexer->context->file);

    // set line and column
    Token next_token = lexer_lookahead(lexer, 1);
    ((ASTNode*)node)->line = next_token.line;
    ((ASTNode*)node)->column = next_token.column;

    if (parser_parse_type(lexer, &((ASTNode*)node)->type) != E_SUCCESS) {
        return parser_error(lexer, "Expected variable type.");
    }

    if (parser_parse_id(lexer, &node->identifier) != E_SUCCESS) {
        return parser_error(lexer, "Expected variable name.");
    }

    ast_add_child(parent, node);
    return parser_parse_var_id_list_tail(lexer, ((ASTNode*)node)->type, parent);
}

/**
 * <var_id_list_tail> -> , <id> <var_id_list_tail> | ;
 */
Error parser_parse_var_id_list_tail(Lexer* lexer, DataType type, ASTNode* parent)
{
    Token token = lexer_next(lexer);
    if (token.type == T_COMMA) {
        ASTDecl* node = ast_create_node(AST_VAR_DECL, lexer->context->file);
        ((ASTNode*)node)->type = type;

        // set line and column
        ((ASTNode*)node)->line = token.line;
        ((ASTNode*)node)->column = token.column;

        //first derivation
        if (parser_parse_id(lexer, &node->identifier) != E_SUCCESS) {
            return parser_error(lexer, "Expected identifier.");
        }

        ast_add_child(parent, node);

        if (parser_parse_var_id_list_tail(lexer, type, parent) != E_SUCCESS) {
            return parser_error(lexer, "Expected variable identifier list tail.");
        }
    } else if (token.type != T_STATEMENT_END) {
        return parser_error(lexer, "Missing semicolon ';' or comma ',' after variable declaration.");
    }

    return E_SUCCESS;
}

/**
 * <type> -> int | boolean
 */
Error parser_parse_type(Lexer* lexer, DataType* type)
{
    Token token = lexer_next(lexer);

    if (token.type == T_BOOLEAN) {
        *type = TYPE_BOOLEAN;
    } else if (token.type == T_INT) {
        *type = TYPE_INT;
    } else {
        return E_PARSE_ERROR;
    }

    return E_SUCCESS;
}

/**
 * <statement> -> <location> <assign_op> <expr> ;
 *              | <method_call> ;
 *              | if ( <expr> ) <block> <else_expr>
 *              | for <id> = <expr> , <expr> <block>
 *              | return <expr_option> ;
 *              | break ;
 *              | continue ;
 *              | <block>
 */
Error parser_parse_statement(Lexer* lexer, ASTNode** node)
{
    Token token = lexer_lookahead(lexer, 1);

    // second derivation - method call
    if (token.type == T_CALLOUT || (token.type == T_IDENTIFIER && lexer_lookahead(lexer, 2).type == T_PAREN_LEFT)) {
        if (parser_parse_method_call(lexer, (ASTReference**)node) != E_SUCCESS) {
            return parser_error(lexer, "Expected method call.");
        }

        if (lexer_next(lexer).type != T_STATEMENT_END) {
            return parser_error(lexer, "Missing semicolon at end of statement.");
        }

        return E_SUCCESS;
    }

    // first derivation - assignment statement
    else if (token.type == T_IDENTIFIER) {
        // Parse the location first. The location will be the first child of the
        // assignment operation node once we parse it.
        ASTReference* location;
        if (parser_parse_location(lexer, &location) != E_SUCCESS) {
            return parser_error(lexer, "Expected location in statement.");
        }

        // now parse the assignment node, and add the location as the left
        // operand
        if (parser_parse_assign_op(lexer, (ASTOperation**)node) != E_SUCCESS) {
            return parser_error(lexer, "Expected assignment operator.");
        }
        ast_add_child(*node, location);

        // now parse the right operand expression
        ASTNode* expr;
        if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
            return parser_error(lexer, "Expected expression.");
        }
        ast_add_child(*node, expr);

        // semicolon
        if (lexer_next(lexer).type != T_STATEMENT_END) {
            return parser_error(lexer, "Missing semicolon at end of statement.");
        }

        return E_SUCCESS;
    }

    // eighth derivation
    else if (token.type == T_BRACE_LEFT) {
        if (parser_parse_block(lexer, node) != E_SUCCESS) {
            return parser_error(lexer, "Expected block.");
        }

        return E_SUCCESS;
    }

    // terminals
    lexer_next(lexer);
    // third derivation - if statement
    if (token.type == T_IF) {
        *node = ast_create_node(AST_IF_STATEMENT, lexer->context->file);

        // set line and column
        ((ASTNode*)*node)->line = token.line;
        ((ASTNode*)*node)->column = token.column;

        if (lexer_next(lexer).type != T_PAREN_LEFT) {
            return parser_error(lexer, "Missing opening parenthesis.");
        }

        ASTNode* expr;
        if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
            return parser_error(lexer, "Expected expression.");
        }
        ast_add_child(*node, expr);

        if (lexer_next(lexer).type != T_PAREN_RIGHT) {
            return parser_error(lexer, "Missing closing parenthesis.");
        }

        ASTNode* block;
        if (parser_parse_block(lexer, &block) != E_SUCCESS) {
            return parser_error(lexer, "Expected block.");
        }
        ast_add_child(*node, block);

        if (parser_parse_else_expr(lexer, *node) != E_SUCCESS) {
            return parser_error(lexer, "Expected else expression.");
        }

        return E_SUCCESS;
    }

    // fourth derivation
    else if (token.type == T_FOR) {
        *node = ast_create_node(AST_FOR_STATEMENT, lexer->context->file);

        // set line and column
        (*node)->line = token.line;
        (*node)->column = token.column;

        // variable used in the loop
        ASTDecl* var = ast_create_node(AST_VAR_DECL, lexer->context->file);
        // is always an int
        ((ASTNode*)var)->type = TYPE_INT;

        // set line and column
        Token next_token = lexer_lookahead(lexer, 1);
        ((ASTNode*)var)->line = next_token.line;
        ((ASTNode*)var)->column = next_token.column;

        // get the variable id
        if (parser_parse_id(lexer, &var->identifier) != E_SUCCESS) {
            return parser_error(lexer, "Expected variable name.");
        }
        ast_add_child(*node, var);

        // the variable is declared and assigned to in one go; create the
        // assignment node now
        ASTOperation* assignment = ast_create_node(AST_ASSIGN_OP, lexer->context->file);
        // get the operator
        Token operator_token = lexer_next(lexer);
        if (operator_token.type != T_EQUAL) {
            return parser_error(lexer, "Expected equals '=' sign.");
        }
        assignment->operator = operator_token.lexeme;
        ast_add_child(*node, assignment);

        // set line and column
        ((ASTNode*)assignment)->line = operator_token.line;
        ((ASTNode*)assignment)->column = operator_token.column;

        // now make the "location" node - the location assigned to
        ASTReference* location = ast_create_node(AST_LOCATION, lexer->context->file);
        // variable name is same as in declaration
        location->identifier = var->identifier;
        ast_add_child(assignment, location);

        // set line and column
        ((ASTNode*)location)->line = next_token.line;
        ((ASTNode*)location)->column = next_token.column;

        // get the assignment value expression
        ASTNode* expr;
        if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
            return parser_error(lexer, "Expected expression.");
        }
        ast_add_child(assignment, expr);

        if (lexer_next(lexer).type != T_COMMA) {
            return parser_error(lexer, "Expected comma ',' after expression.");
        }

        if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
            return parser_error(lexer, "Expected expression.");
        }
        ast_add_child(*node, expr);

        ASTNode* block;
        if (parser_parse_block(lexer, &block) != E_SUCCESS) {
            return parser_error(lexer, "Expected block.");
        }
        ast_add_child(*node, block);

        return E_SUCCESS;
    }

    // fifth derivation - return statement
    else if (token.type == T_RETURN) {
        *node = ast_create_node(AST_RETURN_STATEMENT, lexer->context->file);

        // set line and column
        (*node)->line = token.line;
        (*node)->column = token.column;

        if (parser_parse_expr_option(lexer, *node) != E_SUCCESS) {
            return E_PARSE_ERROR;
        }

        if (lexer_next(lexer).type != T_STATEMENT_END) {
            return parser_error(lexer, "Missing semicolon at end of statement.");
        }

        return E_SUCCESS;
    }

    // sixth derivation
    else if (token.type == T_BREAK) {
        *node = ast_create_node(AST_BREAK_STATEMENT, lexer->context->file);

        // set line and column
        (*node)->line = token.line;
        (*node)->column = token.column;

        if (lexer_next(lexer).type != T_STATEMENT_END) {
            return parser_error(lexer, "Missing semicolon at end of statement.");
        }

        return E_SUCCESS;
    }

    // seventh derivation
    else if (token.type == T_CONTINUE) {
        *node = ast_create_node(AST_CONTINUE_STATEMENT, lexer->context->file);

        // set line and column
        (*node)->line = token.line;
        (*node)->column = token.column;

        if (lexer_next(lexer).type != T_STATEMENT_END) {
            return parser_error(lexer, "Missing semicolon at end of statement.");
        }

        return E_SUCCESS;
    }

    return parser_error(lexer, "Invalid statement.");
}

/**
 * <else_expr> -> else <block> | EPSILON
 */
Error parser_parse_else_expr(Lexer* lexer, ASTNode* parent)
{
    Token token = lexer_lookahead(lexer, 1);

    // first derivation
    if (token.type == T_ELSE) {
        lexer_next(lexer);

        // create an else node
        ASTNode* else_expr = ast_create_node(AST_ELSE_STATEMENT, lexer->context->file);
        ast_add_child(parent, else_expr);

        // set line and column
        else_expr->line = token.line;
        else_expr->column = token.column;

        // parse the else's block
        ASTNode* block;
        if (parser_parse_block(lexer, &block) != E_SUCCESS) {
            return parser_error(lexer, "Expected block following else statement.");
        }
        ast_add_child(else_expr, block);
    }

    // epsilon
    return E_SUCCESS;
}

/**
 * <expr_option> -> <expr> | EPSILON
 */
Error parser_parse_expr_option(Lexer* lexer, ASTNode* parent)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_STATEMENT_END) {
        return E_SUCCESS;
    }

    // expect an expression
    ASTNode* expr;
    if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }
    ast_add_child(parent, expr);

    return E_SUCCESS;
}

/**
 * <assign_op> -> = | += | -=
 */
Error parser_parse_assign_op(Lexer* lexer, ASTOperation** node)
{
    // I think I got this
    Token token = lexer_next(lexer);
    if (token.type != T_EQUAL && token.type != T_PLUS_EQUAL && token.type != T_MINUS_EQUAL) {
        return parser_error(lexer, "Expected an assignment operator ('=', '+=', '-=').");
    }

    *node = ast_create_node(AST_ASSIGN_OP, lexer->context->file);
    (*node)->operator = token.lexeme;

    // set line and column
    ((ASTNode*)*node)->line = token.line;
    ((ASTNode*)*node)->column = token.column;

    return E_SUCCESS;
}

/**
 * <method_call> -> <method_name> ( <expr_list> )
 *                | callout ( <string_literal> <callout_arg_list> )
 */
Error parser_parse_method_call(Lexer* lexer, ASTReference** node)
{
    Token first_token = lexer_lookahead(lexer, 1);

    // library callout call
    if (first_token.type == T_CALLOUT) {
        lexer_next(lexer);
        *node = ast_create_node(AST_CALLOUT, lexer->context->file);

        // set line and column
        ((ASTNode*)*node)->line = first_token.line;
        ((ASTNode*)*node)->column = first_token.column;

        // we know the return type already; is always int
        ((ASTNode*)*node)->type = TYPE_INT;
    } else {
        // standard method call
        *node = ast_create_node(AST_METHOD_CALL, lexer->context->file);

        // set line and column
        ((ASTNode*)*node)->line = first_token.line;
        ((ASTNode*)*node)->column = first_token.column;

        // parse the method name
        if (parser_parse_method_name(lexer, &(*node)->identifier) != E_SUCCESS) {
            return parser_error(lexer, "Expected method name in method call.");
        }
    }

    // left paren
    if (lexer_next(lexer).type != T_PAREN_LEFT) {
        return parser_error(lexer, "Missing opening parenthesis in method call.");
    }

    // callout arguments
    if (first_token.type == T_CALLOUT) {
        // callout expects a string first
        ASTNode* string_literal;
        if (parser_parse_string_literal(lexer, &string_literal) != E_SUCCESS) {
            return parser_error(lexer, "Expected library function name in callout.");
        }
        (*node)->identifier = (char*)string_literal->value;

        // parse the arguments, if any
        if (parser_parse_callout_arg_list(lexer, *node) != E_SUCCESS) {
            return parser_error(lexer, "Expected argument list in callout.");
        }
    // regular method call arguments
    } else if (parser_parse_expr_list(lexer, (ASTNode*)*node) != E_SUCCESS) {
        return parser_error(lexer, "Expected argument list in method call.");
    }

    // right paren
    if (lexer_next(lexer).type != T_PAREN_RIGHT) {
        return parser_error(lexer, "Missing closing parenthesis in method call.");
    }

    return E_SUCCESS;
}

/**
 * <expr_list> -> <expr> <expr_list_tail> | EPSILON
 */
Error parser_parse_expr_list(Lexer* lexer, ASTNode* parent)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_PAREN_RIGHT) {
        return E_SUCCESS;
    }

    // parse an expr and add it to the parent node
    ASTNode* expr;
    if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }
    ast_add_child(parent, expr);

    // see if there are more expressions to parse
    return parser_parse_expr_list_tail(lexer, parent);
}

/**
 * <expr_list_tail> -> , <expr> <expr_list_tail> | EPSILON
 */
Error parser_parse_expr_list_tail(Lexer* lexer, ASTNode* parent)
{
    Token first_token = lexer_lookahead(lexer, 1);

    // no more commas - end of expr list
    if (first_token.type != T_COMMA) {
        // epsilon derivation
        return E_SUCCESS;
    }

    // consume the comma
    lexer_next(lexer);

    // parse another expr and add it to the parent

    ASTNode* expr;
    if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
        return parser_error(lexer, "Expected another expression following comma.");
    }
    ast_add_child(parent, expr);

    // see if there are more expressions to parse
    return parser_parse_expr_list_tail(lexer, parent);
}

/**
 * <callout_arg_list> -> , <callout_arg> <callout_arg_list> | EPSILON
 */
Error parser_parse_callout_arg_list(Lexer* lexer, ASTReference* parent)
{
    Token token = lexer_lookahead(lexer, 1);

    // first derivation
    if (token.type == T_COMMA) {
        lexer_next(lexer);

        ASTNode* arg;
        if (parser_parse_callout_arg(lexer, &arg) != E_SUCCESS) {
            return parser_error(lexer, "Expected another argument in callout argument list.");
        }
        ast_add_child(parent, arg);

        // parse more arguments if there are any
        return parser_parse_callout_arg_list(lexer, parent);
    }

    // epsilon
    return E_SUCCESS;
}

/**
 * <method_name> -> <id>
 */
Error parser_parse_method_name(Lexer* lexer, char** identifier)
{
    return parser_parse_id(lexer, identifier);
}

/**
 * <location> -> <id> <array_subscript_expr>
 */
Error parser_parse_location(Lexer* lexer, ASTReference** node)
{
    *node = ast_create_node(AST_LOCATION, lexer->context->file);
    Token token = lexer_lookahead(lexer, 1);

    // set line and column
    ((ASTNode*)*node)->line = token.line;
    ((ASTNode*)*node)->column = token.column;

    if (parser_parse_id(lexer, &(*node)->identifier) != E_SUCCESS) {
        return parser_error(lexer, "Failure in parsing location - parser_parse_id failed.");
    }

    return parser_parse_array_subscript_expr(lexer, *node);
}

/**
 * <array_subscript_expr> -> [ <expr> ] | EPSILON
 */
Error parser_parse_array_subscript_expr(Lexer* lexer, ASTReference* parent)
{
    Token token = lexer_lookahead(lexer, 1);

    if (token.type == T_BRACKET_LEFT) {
        // consume current token
        lexer_next(lexer);

        ASTNode* expr;
        if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
            return parser_error(lexer, "Expected expression inside array subscript.");
        }
        ast_add_child(parent, expr);

        if (lexer_next(lexer).type != T_BRACKET_RIGHT) {
            return parser_error(lexer, "Missing closing bracket in array subscript expression.");
        }
    }

    // epsilon
    return E_SUCCESS;
}

/**
 * Note that this function parses the following 2 grammar rules at once:
 *
 *     <expr> -> <expr_part> <expr_end>
 *     <expr_end> -> <bin_op> <expr> | EPSILON
 */
Error parser_parse_expr(Lexer* lexer, ASTNode** node)
{
    // Create a node for the expression parsed. We don't know yet if this
    // expression is an operand for a binary operation or not.
    ASTNode* left_expr;

    // parse the primary expression
    if (parser_parse_expr_part(lexer, &left_expr) != E_SUCCESS) {
        return E_PARSE_ERROR;
    }

    // Now check if there is a binary operator following the above expression.
    // If there is, try and parse it and make it the parent node for the left
    // and right operand expressions.
    if (token_is_bin_op(lexer_lookahead(lexer, 1))) {
        // We do have a binary operation, so set the following operation node
        // as the root node of the expression.
        if (parser_parse_bin_op(lexer, (ASTOperation**)node) != E_SUCCESS) {
            return parser_error(lexer, "Expected binary operator.");
        }

        // Now add the original expression we parsed earlier as the left
        // operand expression.
        ast_add_child(*node, left_expr);

        // Now parse the right operand expression and add it to the parent
        // operation node.
        ASTNode* right_expr;
        if (parser_parse_expr(lexer, &right_expr) != E_SUCCESS) {
            return parser_error(lexer, "Expected expression.");
        }
        ast_add_child(*node, right_expr);
    }

    // If a binary operator was not found, the left expression is the only
    // expression and becomes the root node. This is equivalent to the non-
    // terminal <expr_end> deriving as epsilon.
    else {
        *node = left_expr;
    }

    return E_SUCCESS;
}

/**
 * Parses the primary part of an expression.
 *
 * <expr_part> -> <location>
 *              | <method_call>
 *              | <literal>
 *              | - <expr>
 *              | ! <expr>
 *              | ( <expr> )
 */
Error parser_parse_expr_part(Lexer* lexer, ASTNode** node)
{
    Token next_token = lexer_lookahead(lexer, 1);

    // second derivation - method call
    if (next_token.type == T_CALLOUT || (next_token.type == T_IDENTIFIER && lexer_lookahead(lexer, 2).type == T_PAREN_LEFT)) {

        if (parser_parse_method_call(lexer, (ASTReference**)node) != E_SUCCESS) {
            return parser_error(lexer, "Expected method call.");
        }

        return E_SUCCESS;
    }

    // first derivation - location
    if (next_token.type == T_IDENTIFIER) {
        if (parser_parse_location(lexer, (ASTReference**)node) != E_SUCCESS) {
            return parser_error(lexer, "Expected location.");
        }

        return E_SUCCESS;
    }

    // fourth and fifth derivation - unary operation
    if (next_token.type == T_MINUS || next_token.type == T_LOGICAL_NOT) {
        lexer_next(lexer);

        // create a unary expression node
        *node = ast_create_node(AST_UNARY_OP, lexer->context->file);
        ((ASTOperation*)*node)->operator = next_token.lexeme;

        // set line and column
        (*node)->line = next_token.line;
        (*node)->column = next_token.column;

        // Parse a sub-expression and add it as the only child of the unary
        // operation node.
        ASTNode* expr;
        if (parser_parse_expr(lexer, &expr) != E_SUCCESS) {
            return parser_error(lexer, "Expected expression.");
        }
        ast_add_child(*node, expr);

        return E_SUCCESS;
    }

    // last derivation - a sub-expression in parenthesis
    if (next_token.type == T_PAREN_LEFT) {
        lexer_next(lexer);

        if (parser_parse_expr(lexer, node) != E_SUCCESS) {
            return parser_error(lexer, "Expected expression.");
        }

        if (lexer_next(lexer).type != T_PAREN_RIGHT) {
            return parser_error(lexer, "Missing closing parenthesis.");
        }

        return E_SUCCESS;
    }

    // third derivation - a simple literal
    if (parser_parse_literal(lexer, node) != E_SUCCESS) {
        return parser_error(lexer, "Expected literal expression.");
    }

    return E_SUCCESS;
}

/**
 * <callout_arg> -> <expr> | <string_literal>
 */
Error parser_parse_callout_arg(Lexer* lexer, ASTNode** node)
{
    // second derivation - string literal
    if (lexer_lookahead(lexer, 1).type == T_STRING_LITERAL) {
        // parse a string and use that as the argument
        if (parser_parse_string_literal(lexer, node) != E_SUCCESS) {
            return parser_error(lexer, "Expected string literal.");
        }
    }

    // first derivation
    else if (parser_parse_expr(lexer, node) != E_SUCCESS) {
        return parser_error(lexer, "Expected expression.");
    }

    return E_SUCCESS;
}

/**
 * <bin_op> -> <arith_op> | <rel_op> | <eq_op> | <cond_op>
 */
Error parser_parse_bin_op(Lexer* lexer, ASTOperation** node)
{
    // make sure next token is a binary operator
    Token token = lexer_next(lexer);
    if (!token_is_bin_op(token)) {
        return parser_error(lexer, "Invalid operator type.");
    }

    // create a binary op node
    *node = ast_create_node(AST_BINARY_OP, lexer->context->file);

    // set line and column
    ((ASTNode*)*node)->line = token.line;
    ((ASTNode*)*node)->column = token.column;

    // get the operator from the token lexeme
    (*node)->operator = token.lexeme;

    return E_SUCCESS;
}

/**
 * <literal> -> <int_literal> | <char_literal> | <bool_literal>
 */
Error parser_parse_literal(Lexer* lexer, ASTNode** node)
{
    Token token = lexer_lookahead(lexer, 1);

    if (token.type == T_INT_LITERAL) {
        if (parser_parse_int_literal(lexer, node) != E_SUCCESS) {
            return parser_error(lexer, "Error in parsing literal - parsing failed at parser_parse_int_literal.");
        }
    }

    else if (token.type == T_CHAR_LITERAL) {
        if (parser_parse_char_literal(lexer, node) != E_SUCCESS) {
            return parser_error(lexer, "Error in parsing literal - parsing failed at parser_parse_char_literal.");
        }
    }

    else if (parser_parse_bool_literal(lexer, node) != E_SUCCESS) {
        return parser_error(lexer, "Error in parsing literal - parsing failed at parser_parse_bool_literal.");
    }

    return E_SUCCESS;
}

/**
 * <id> -> <alpha> <alpha_num_string>
 */
Error parser_parse_id(Lexer* lexer, char** identifier)
{
    Token token = lexer_next(lexer);

    if (token.type != T_IDENTIFIER) {
        return E_PARSE_ERROR;
    }

    *identifier = token.lexeme;
    return E_SUCCESS;
}

/**
 * <int_literal> -> <decimal_literal> | <hex_literal>
 */
Error parser_parse_int_literal(Lexer* lexer, ASTNode** node)
{
    Token token = lexer_next(lexer);
    if (token.type != T_INT_LITERAL) {
        return parser_error(lexer, "Expected an integer literal.");
    }

    // create a node
    *node = ast_create_node(AST_INT_LITERAL, lexer->context->file);
    (*node)->type = TYPE_INT;

    // set line and column
    (*node)->line = token.line;
    (*node)->column = token.column;

    // get the actual int value
    (*node)->value = malloc(sizeof(int));
    *(int*)(*node)->value = parser_str_to_long(token.lexeme);

    return E_SUCCESS;
}

/**
 * <bool_literal> -> true | false
 */
Error parser_parse_bool_literal(Lexer* lexer, ASTNode** node)
{
    Token token = lexer_next(lexer);
    if (token.type != T_BOOLEAN_LITERAL) {
        return parser_error(lexer, "Expected 'true' or 'false'.");
    }

    // create a node
    *node = ast_create_node(AST_BOOLEAN_LITERAL, lexer->context->file);
    (*node)->type = TYPE_BOOLEAN;

    // set line and column
    (*node)->line = token.line;
    (*node)->column = token.column;

    // get the actual boolean value
    (*node)->value = malloc(sizeof(bool));
    if (strcmp(token.lexeme, "true") == 0) {
        *(bool*)(*node)->value = true;
    } else {
        *(bool*)(*node)->value = false;
    }

    return E_SUCCESS;
}

/**
 * <char_literal> -> ' <char> '
 */
Error parser_parse_char_literal(Lexer* lexer, ASTNode** node)
{
    Token token = lexer_next(lexer);
    if (token.type != T_CHAR_LITERAL) {
        return parser_error(lexer, "Expected a char literal.");
    }

    // create a node
    *node = ast_create_node(AST_CHAR_LITERAL, lexer->context->file);
    (*node)->type = TYPE_CHAR;

    // set line and column
    (*node)->line = token.line;
    (*node)->column = token.column;

    // get the actual value
    (*node)->value = parser_strip_quotes(token.lexeme);

    return E_SUCCESS;
}

/**
 * <string_literal> -> " <char> "
 */
Error parser_parse_string_literal(Lexer* lexer, ASTNode** node)
{
    Token token = lexer_next(lexer);
    if (token.type != T_STRING_LITERAL) {
        return parser_error(lexer, "Expected a string literal.");
    }

    // create a node
    *node = ast_create_node(AST_STRING_LITERAL, lexer->context->file);
    (*node)->type = TYPE_STRING;

    // set line and column
    (*node)->line = token.line;
    (*node)->column = token.column;

    // get the actual value
    (*node)->value = parser_strip_quotes(token.lexeme);

    return E_SUCCESS;
}
