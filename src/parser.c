#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "lexer.h"
#include "parser.h"


/**
 * Parses the tokens yielded by a given lexer.
 */
Error parser_parse(Lexer* lexer)
{
    // the source file should contain a single program (duh!)
    if (!parser_parse_program(lexer)) {
        char buffer[512];
        sprintf(buffer, "Failed to parse file \"%s\".", lexer->context->file);
        return error(E_PARSE_ERROR, buffer);
    }

    return E_SUCCESS;
}

/**
 * Displays a parser error at the current token.
 */
Error parser_error(Lexer* lexer, char* message)
{
    // get the current token
    Token token = lexer->current_node->token;

    // make a new message string containing extra info
    char* full_message = (char*)malloc(64 + strlen(token.file) + strlen(message));
    sprintf(full_message, "in file \"%s\" near line %d, column %d:\n\t%s", token.file, token.line, token.column, message);

    // advance to the end of the statement
    while (token.type != T_STATEMENT_END && token.type != T_EOF) {
        token = lexer_next(lexer);
    }

    return error(E_PARSE_ERROR, full_message);
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
bool parser_parse_program(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_CLASS) {
        parser_error(lexer, "A program starts with 'class', you fool.");
        return false;
    }

    token = lexer_next(lexer);
    if (token.type != T_IDENTIFIER || strcmp(token.lexeme, "Program") != 0) {
        parser_error(lexer, "Expecting 'Program'.");
        return false;
    }

    token = lexer_next(lexer);
    if (token.type != T_BRACE_LEFT) {
        parser_error(lexer, "Missing opening curly brace for class 'Program'.");
        return false;
    }

    // both can be empty, so no errors unique to here
    if (!parser_parse_field_decl_list(lexer)) {
        return false;
    }
    if (!parser_parse_method_decl_list(lexer)) {
        return false;
    }

    token = lexer_next(lexer);
    if (token.type != T_BRACE_RIGHT) {
        parser_error(lexer, "Missing closing curly brace for class 'Program'.");
        return false;
    }

    if (lexer_next(lexer).type != T_EOF) {
        parser_error(lexer, "Expected end of file.");
        return false;
    }

    return true;
}

/**
 * <field_decl_list> -> <field_decl> <field_decl_list> | EPSILON
 */
bool parser_parse_field_decl_list(Lexer* lexer)
{
    // do a lookahead
    Token token = lexer_lookahead(lexer, 1);

    // try to parse a field decl?
    if (token.type == T_BOOLEAN || token.type == T_INT) {
        // might be a field decl, do a further lookahead
        token = lexer_lookahead(lexer, 3);

        if (token.type != T_PAREN_LEFT) {
            if (!parser_parse_field_decl(lexer)) {
                return false;
            }

            // if that worked, we must have a field_decl_list next
            if (!parser_parse_field_decl_list(lexer)) {
                return false;
            }
        }
    }

    // empty string works
    return true;
}

/**
 * <method_decl_list> -> <method_decl> <method_decl_list> | EPSILON
 */
bool parser_parse_method_decl_list(Lexer* lexer)
{
    Token first_token = lexer_lookahead(lexer, 1);

    // epsilon
    if (first_token.type == T_BRACE_RIGHT) {
        return true;
    }

    // try to parse a method decl
    if (!parser_parse_method_decl(lexer)) {
        return false;
    }
    // if that worked, we must have a method_decl_list
    if (!parser_parse_method_decl_list(lexer)) {
        return false;
    }

    return true;
}

/**
 * <field_decl> -> <type> <field_id_list>
 */
bool parser_parse_field_decl(Lexer* lexer)
{
    return parser_parse_type(lexer)
        && parser_parse_field_id_list(lexer);
}

/**
 * <field_id_list> -> <id> <array_dim_decl> <field_id_list_tail>
 */
bool parser_parse_field_id_list(Lexer* lexer)
{
    if (!parser_parse_id(lexer)) {
        parser_error(lexer, "Expected field name.");
        return false;
    }
    return parser_parse_array_dim_decl(lexer)
        && parser_parse_field_id_list_tail(lexer);
}

/**
 * <array_dim_decl> -> [ <int_literal> ] | EPSILON
 */
bool parser_parse_array_dim_decl(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    if (token.type == T_BRACKET_LEFT) {
        lexer_next(lexer);

        if (!parser_parse_int_literal(lexer)) {
            parser_error(lexer, "Expected array length.");
            return false;
        }

        token = lexer_next(lexer);
        if (token.type != T_BRACKET_RIGHT) {
            parser_error(lexer, "Missing closing bracket in array declaration.");
            return false;
        }
    }

    // empty string works
    return true;
}

/**
 * <field_id_list_tail> -> , <field_id_list> | ;
 */
bool parser_parse_field_id_list_tail(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type == T_COMMA) {
        //first derivation
        if (!parser_parse_field_id_list(lexer)) {
            parser_error(lexer, "Expected field id list.");
            return false;
        }

        return true;
    } else if (token.type == T_STATEMENT_END) {
        return true;
    }

    parser_error(lexer, "Missing semicolon ';' or comma ',' after field declaration.");
    return false;
}

/**
 * <method_decl> -> <type> <id> ( <method_param_decl_list> ) <block>
                  | void <id> ( <method_param_decl_list> ) <block>
 */
bool parser_parse_method_decl(Lexer* lexer)
{
    // can start with <type> or void
    if (lexer_lookahead(lexer, 1).type == T_VOID) {
        lexer_next(lexer);
    } else if (!parser_parse_type(lexer)) {
        parser_error(lexer, "Expected type name or void.");
        return false;
    }

    // must have an identifier next
    if (!parser_parse_id(lexer)) {
        parser_error(lexer, "Expected identifier.");
        return false;
    }

    // left paren next ...
    if (lexer_next(lexer).type != T_PAREN_LEFT) {
        parser_error(lexer, "Expected left parentheses when parsing method_decl and did not get one.");
        return false;
    }

    // ... then the params ...
    if (!parser_parse_method_param_decl_list(lexer)) {
        parser_error(lexer, "Expected method argument list.");
        return false;
    }

    // ... right paren next ...
    if (lexer_next(lexer).type != T_PAREN_RIGHT) {
        parser_error(lexer, "Expected right parentheses when parsing method_decl and did not get one.");
        return false;
    }

    // ... then finally a block
    if (!parser_parse_block(lexer)) {
        return false;
    }

    // we made it!
    return true;
}

/**
 * <method_param_decl_list> -> <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
bool parser_parse_method_param_decl_list(Lexer* lexer)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_PAREN_RIGHT) {
        return true;
    }

    if (!parser_parse_method_param_decl(lexer)) {
        parser_error(lexer, "Expected parameter declaration.");
        return false;
    }

    if (!parser_parse_method_param_decl_list_tail(lexer)) {
        parser_error(lexer, "Expected parameter list tail.");
        return false;
    }

    return true;
}

/**
 * <method_param_decl_list_tail> -> , <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
bool parser_parse_method_param_decl_list_tail(Lexer* lexer)
{
    // first derivation
    if (lexer_lookahead(lexer, 1).type == T_COMMA) {
        lexer_next(lexer);

        if (!parser_parse_method_param_decl(lexer)) {
            parser_error(lexer, "Expected method parameter declaration.");
            return false;
        }

        if (!parser_parse_method_param_decl_list_tail(lexer)) {
            parser_error(lexer, "Expected parameter list tail.");
            return false;
        }
    }

    // epsilon
    return true;
}

/**
 * <method_param_decl> -> <type> <id>
 */
bool parser_parse_method_param_decl(Lexer* lexer)
{
    if (!parser_parse_type(lexer)) {
        parser_error(lexer, "Expected parameter type.");
        return false;
    }

    if (!parser_parse_id(lexer)) {
        parser_error(lexer, "Expected parameter identifier.");
        return false;
    }

    return true;
}

/**
 * <block> -> { <var_decl_list> <statement_list> }
 */
bool parser_parse_block(Lexer* lexer)
{
    if (lexer_next(lexer).type != T_BRACE_LEFT) {
        parser_error(lexer, "Missing left curly brace when parsing block.");
        return false;
    }

    if (!parser_parse_var_decl_list(lexer)) {
        return false;
    }

    if (!parser_parse_statement_list(lexer)) {
        return false;
    }

    if (lexer_next(lexer).type != T_BRACE_RIGHT) {
        parser_error(lexer, "Missing right curly brace when parsing block.");
        return false;
    }

    return true;
}

/**
 * <var_decl_list> -> <var_decl> <var_decl_list> | EPSILON
 */
bool parser_parse_var_decl_list(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    // first derivation
    if (token.type == T_BOOLEAN || token.type == T_INT) {
        if (!parser_parse_var_decl(lexer)) {
            return false;
        }

        if (!parser_parse_var_decl_list(lexer)) {
            return false;
        }
    }

    // epsilon
    return true;
}

/**
 * <statement_list> -> <statement> <statement_list> | EPSILON
 */
bool parser_parse_statement_list(Lexer* lexer)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_BRACE_RIGHT) {
        return true;
    }

    // first derivation
    if (!parser_parse_statement(lexer)) {
        return false;
    }

    if (!parser_parse_statement_list(lexer)) {
        return false;
    }

    return true;
}

/**
 * <var_decl> -> <type> <id> <var_id_list_tail>
 */
bool parser_parse_var_decl(Lexer* lexer)
{
    if (!parser_parse_type(lexer)) {
        parser_error(lexer, "Expected variable type.");
        return false;
    }

    if (!parser_parse_id(lexer)) {
        parser_error(lexer, "Expected variable name.");
        return false;
    }

    return parser_parse_var_id_list_tail(lexer);
}

/**
 * <var_id_list_tail> -> , <id> <var_id_list_tail> | ;
 */
bool parser_parse_var_id_list_tail(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type == T_COMMA) {
        //first derivation
        if (!parser_parse_id(lexer)) {
            parser_error(lexer, "Expected identifier.");
            return false;
        }

        if (!parser_parse_var_id_list_tail(lexer)) {
            parser_error(lexer, "Expected variable identifier list tail.");
            return false;
        }
        return true;

    } else if (token.type == T_STATEMENT_END) {
        return true;
    }

    parser_error(lexer, "Missing semicolon ';' or comma ',' after variable declaration.");
    return false;
}

/**
 * <type> -> int | boolean
 */
bool parser_parse_type(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    return token.type == T_BOOLEAN || token.type == T_INT;
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
bool parser_parse_statement(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    // second derivation - method call
    if (token.type == T_CALLOUT || (token.type == T_IDENTIFIER && lexer_lookahead(lexer, 2).type == T_PAREN_LEFT)) {
        if (!parser_parse_method_call(lexer)) {
            parser_error(lexer, "Expected method call.");
            return false;
        }

        if (lexer_next(lexer).type != T_STATEMENT_END) {
            parser_error(lexer, "Missing semicolon at end of statement.");
            return false;
        }

        return true;
    }

    // first derivation - location...
    else if (token.type == T_IDENTIFIER) {
        if (!parser_parse_location(lexer)) {
            parser_error(lexer, "Expected location in statement.");
            return false;
        }

        if (!parser_parse_assign_op(lexer)) {
            parser_error(lexer, "Expected assignment operator.");
            return false;
        }

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression.");
            return false;
        }

        if (lexer_next(lexer).type != T_STATEMENT_END) {
            parser_error(lexer, "Missing semicolon at end of statement.");
            return false;
        }

        return true;
    }

    // eighth derivation
    else if (token.type == T_BRACE_LEFT) {
        if (!parser_parse_block(lexer)) {
            parser_error(lexer, "Expected block.");
            return false;
        }

        return true;
    }

    // terminals
    lexer_next(lexer);
    // third derivation - if statement
    if (token.type == T_IF) {
        if (lexer_next(lexer).type != T_PAREN_LEFT) {
            parser_error(lexer, "Missing opening parenthesis.");
            return false;
        }

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression.");
            return false;
        }

        if (lexer_next(lexer).type != T_PAREN_RIGHT) {
            parser_error(lexer, "Missing closing parenthesis.");
            return false;
        }

        if (!parser_parse_block(lexer)) {
            parser_error(lexer, "Expected block.");
            return false;
        }

        if (!parser_parse_else_expr(lexer)) {
            parser_error(lexer, "Expected else expression.");
            return false;
        }

        return true;
    }

    // fourth derivation
    else if (token.type == T_FOR) {
        if (!parser_parse_id(lexer)) {
            parser_error(lexer, "Expected variable name.");
            return false;
        }

        if (lexer_next(lexer).type != T_EQUAL) {
            parser_error(lexer, "Expected equals '=' sign.");
            return false;
        }

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression.");
            return false;
        }

        if (lexer_next(lexer).type != T_COMMA) {
            parser_error(lexer, "Expected comma ',' after expression.");
            return false;
        }

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression.");
            return false;
        }

        if (!parser_parse_block(lexer)) {
            parser_error(lexer, "Expected block.");
            return false;
        }

        return true;
    }

    // fifth derivation
    else if (token.type == T_RETURN) {
        if (!parser_parse_expr_option(lexer)) {
            return false;
        }

        if (lexer_next(lexer).type != T_STATEMENT_END) {
            parser_error(lexer, "Missing semicolon at end of statement.");
            return false;
        }

        return true;
    }

    // sixth and seventh derivations
    else if (token.type == T_BREAK || token.type == T_CONTINUE) {
        if (lexer_next(lexer).type != T_STATEMENT_END) {
            parser_error(lexer, "Missing semicolon at end of statement.");
            return false;
        }

        return true;
    }

    parser_error(lexer, "Invalid statement.");
    return false;
}

/**
 * <else_expr> -> else <block> | EPSILON
 */
bool parser_parse_else_expr(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    // first derivation
    if (token.type == T_ELSE) {
        lexer_next(lexer);

        if (!parser_parse_block(lexer)) {
            parser_error(lexer, "Expected block following else statement.");
            return false;
        }
    }

    // epsilon
    return true;
}

/**
 * <expr_option> -> <expr> | EPSILON
 */
bool parser_parse_expr_option(Lexer* lexer)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_STATEMENT_END) {
        return true;
    }

    // expect an expression
    return parser_parse_expr(lexer);
}

/**
 * <assign_op> -> = | += | -=
 */
bool parser_parse_assign_op(Lexer* lexer)
{
    // I think I got this
    Token token = lexer_next(lexer);
    if (token.type != T_EQUAL && token.type != T_PLUS_EQUAL && token.type != T_MINUS_EQUAL) {
        parser_error(lexer, "Expected an assignment operator ('=', '+=', '-=').");
        return false;
    }
    return true;
}

/**
 * <method_call> -> <method_name> ( <expr_list> )
 *                | callout ( <string_literal> <callout_arg_list> )
 */
bool parser_parse_method_call(Lexer* lexer)
{
    Token first_token = lexer_lookahead(lexer, 1);

    // method name or callout
    if (first_token.type == T_CALLOUT) {
        lexer_next(lexer);
    } else if (!parser_parse_method_name(lexer)) {
        parser_error(lexer, "Expected method name in method call.");
        return false;
    }

    // left paren
    if (lexer_next(lexer).type != T_PAREN_LEFT) {
        parser_error(lexer, "Missing opening parenthesis in method call.");
        return false;
    }

    // method arguments
    if (first_token.type == T_CALLOUT) {
        // callout expects a string first
        if (!parser_parse_string_literal(lexer)) {
            parser_error(lexer, "Expected library function name in callout.");
            return false;
        }

        if (!parser_parse_callout_arg_list(lexer)) {
            parser_error(lexer, "Expected argument list in callout.");
            return false;
        }
    } else if (!parser_parse_expr_list(lexer)) {
        parser_error(lexer, "Expected argument list in method call.");
        return false;
    }

    // right paren
    if (lexer_next(lexer).type != T_PAREN_RIGHT) {
        parser_error(lexer, "Missing closing parenthesis in method call.");
        return false;
    }

    return true;
}

/**
 * <expr_list> -> <expr> <expr_list_tail> | EPSILON
 */
bool parser_parse_expr_list(Lexer* lexer)
{
    // epsilon
    if (lexer_lookahead(lexer, 1).type == T_PAREN_RIGHT) {
        return true;
    }

    // first derivation
    return parser_parse_expr(lexer)
        && parser_parse_expr_list_tail(lexer);
}

/**
 * <expr_list_tail> -> , <expr> <expr_list_tail> | EPSILON
 */
bool parser_parse_expr_list_tail(Lexer* lexer)
{
    Token first_token = lexer_lookahead(lexer, 1);

    //first derivation
    if (first_token.type == T_COMMA) {
        lexer_next(lexer);

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression following comma in expression list.");
            return false;
        }

        if (!parser_parse_expr_list_tail(lexer)) {
            parser_error(lexer, "Expected expression tail following comma in expression list.");
            return false;
        }
    }

    //epsilon derivation
    return true;
}

/**
 * <callout_arg_list> -> , <callout_arg> <callout_arg_list> | EPSILON
 */
bool parser_parse_callout_arg_list(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    // first derivation
    if (token.type == T_COMMA) {
        lexer_next(lexer);

        if (!parser_parse_callout_arg(lexer)) {
            parser_error(lexer, "Expected another argument in callout argument list.");
            return false;
        }

        if (!parser_parse_callout_arg_list(lexer)) {
            parser_error(lexer, "Expected callout argument list.");
            return false;
        }
    }

    // epsilon
    return true;
}

/**
 * <method_name> -> <id>
 */
bool parser_parse_method_name(Lexer* lexer)
{
    return parser_parse_id(lexer);
}

/**
 * <location> -> <id> <array_subscript_expr>
 */
bool parser_parse_location(Lexer* lexer)
{
    if (!parser_parse_id(lexer)) {
        parser_error(lexer, "Failure in parsing location - parser_parse_id failed.");
        return false;
    }

    return parser_parse_array_subscript_expr(lexer);
}

/**
 * <array_subscript_expr> -> [ <expr> ] | EPSILON
 */
bool parser_parse_array_subscript_expr(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    if (token.type == T_BRACKET_LEFT) {
        // consume current token
        lexer_next(lexer);

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression inside array subscript.");
            return false;
        }

        if (lexer_next(lexer).type != T_BRACKET_RIGHT) {
            parser_error(lexer, "Missing closing bracket in array subscript expression.");
            return false;
        }
    }

    // epsilon
    return true;
}

/**
 * <expr> -> <expr_part> <expr_end>
 */
bool parser_parse_expr(Lexer* lexer)
{
    return parser_parse_expr_part(lexer)
        && parser_parse_expr_end(lexer);
}

/**
 * <expr_part> -> <location>
 *              | <method_call>
 *              | <literal>
 *              | - <expr>
 *              | ! <expr>
 *              | ( <expr> )
 */
bool parser_parse_expr_part(Lexer* lexer)
{
    Token next_token = lexer_lookahead(lexer, 1);

    // second derivation - method call
    if (next_token.type == T_CALLOUT || (next_token.type == T_IDENTIFIER && lexer_lookahead(lexer, 2).type == T_PAREN_LEFT)) {
        if (!parser_parse_method_call(lexer)) {
            parser_error(lexer, "Expected method call.");
            return false;
        }

        return true;
    }

    // first derivation - location
    if (next_token.type == T_IDENTIFIER) {
        if (!parser_parse_location(lexer)) {
            parser_error(lexer, "Expected location.");
            return false;
        }

        return true;
    }

    // fourth and fifth derivation
    if (next_token.type == T_MINUS || next_token.type == T_LOGICAL_NOT) {
        lexer_next(lexer);

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression.");
            return false;
        }

        return true;
    }

    // last derivation
    if (next_token.type == T_PAREN_LEFT) {
        lexer_next(lexer);

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression.");
            return false;
        }

        if (lexer_next(lexer).type != T_PAREN_RIGHT) {
            parser_error(lexer, "Missing closing parenthesis.");
            return false;
        }

        return true;
    }

    // third derivation
    if (!parser_parse_literal(lexer)) {
        parser_error(lexer, "Expected literal expression.");
        return false;
    }

    return true;
}

/**
 * <expr_end> -> <bin_op> <expr> | EPSILON
 */
bool parser_parse_expr_end(Lexer* lexer)
{
    Token next_token = lexer_lookahead(lexer, 1);

    // check for <bin_op>
    if (token_is_bin_op(next_token)) {
        // matches! try parsing the rest
        if (!parser_parse_bin_op(lexer)) {
            parser_error(lexer, "Expected binary operator.");
            return false;
        }

        if (!parser_parse_expr(lexer)) {
            parser_error(lexer, "Expected expression.");
            return false;
        }
    }

    // epsilon
    return true;
}

/**
 * <callout_arg> -> <expr> | <string_literal>
 */
bool parser_parse_callout_arg(Lexer* lexer)
{
    // second derivation - string literal
    if (lexer_lookahead(lexer, 1).type == T_STRING_LITERAL) {
        if (!parser_parse_string_literal(lexer)) {
            parser_error(lexer, "Expected string literal.");
            return false;
        }
    }

    // first derivation
    else if (!parser_parse_expr(lexer)) {
        parser_error(lexer, "Expected expression.");
        return false;
    }

    return true;
}

/**
 * <bin_op> -> <arith_op> | <rel_op> | <eq_op> | <cond_op>
 */
bool parser_parse_bin_op(Lexer* lexer)
{
    if (!token_is_bin_op(lexer_next(lexer))) {
        parser_error(lexer, "Invalid operator type.");
        return false;
    }

    return true;
}

/**
 * <literal> -> <int_literal> | <char_literal> | <bool_literal>
 */
bool parser_parse_literal(Lexer* lexer)
{
    Token token = lexer_lookahead(lexer, 1);

    if (token.type == T_INT_LITERAL) {
        if(parser_parse_int_literal(lexer))
            return true;

        parser_error(lexer, "Error in parsing literal - parsing failed at parser_parse_int_literal.");
        return false;
    }

    if (token.type == T_CHAR_LITERAL) {
        if(parser_parse_char_literal(lexer))
            return true;

        parser_error(lexer, "Error in parsing literal - parsing failed at parser_parse_char_literal.");
        return false;
    }

    if(parser_parse_bool_literal(lexer))
        return true;

    //This error could also fire under expected conditions, even if bool_literal doesn't fail, due to the
    //fact that it is technically the default "return false"
    parser_error(lexer, "Error in parsing literal - parsing failed at parser_parse_bool_literal.");
    return false;
}

/**
 * <id> -> <alpha> <alpha_num_string>
 */
bool parser_parse_id(Lexer* lexer)
{
    return lexer_next(lexer).type == T_IDENTIFIER;
}

/**
 * <int_literal> -> <decimal_literal> | <hex_literal>
 */
bool parser_parse_int_literal(Lexer* lexer)
{
    if (lexer_next(lexer).type != T_INT_LITERAL) {
        parser_error(lexer, "Expected an integer literal.");
        return false;
    }

    return true;
}

/**
 * <bool_literal> -> true | false
 */
bool parser_parse_bool_literal(Lexer* lexer)
{
    if (lexer_next(lexer).type != T_BOOLEAN_LITERAL) {
        parser_error(lexer, "Expected 'true' or 'false'.");
        return false;
    }

    return true;
}

/**
 * <char_literal> -> ’ <char> ’
 */
bool parser_parse_char_literal(Lexer* lexer)
{
    if (lexer_next(lexer).type != T_CHAR_LITERAL) {
        parser_error(lexer, "Expected a char literal.");
        return false;
    }

    return true;
}

/**
 * <string_literal> -> " <char> "
 */
bool parser_parse_string_literal(Lexer* lexer)
{
    if (lexer_next(lexer).type != T_STRING_LITERAL) {
        parser_error(lexer, "Expected a string literal.");
        return false;
    }

    return true;
}
