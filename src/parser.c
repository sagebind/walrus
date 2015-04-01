#include <stdbool.h>
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
        return E_PARSE_ERROR;
    }

    return E_SUCCESS;
}

/**
 * Displays a parser error.
 */
Error parser_error(Token token, char* message)
{
    // make a new message string containing extra info
    char* full_message = (char*)malloc(64 + strlen(token.file) + strlen(message));
    sprintf(full_message, "in file \"%s\" near line %d, column %d:\n\t%s", token.file, token.line, token.column, message);
    return error(E_PARSE_ERROR, full_message);
}

/**
 * <program> -> class Program { <field_decl_list> <method_decl_list> }
 */
bool parser_parse_program(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_CLASS) {
        parser_error(token, "A program starts with 'class', you fool.");
        return false;
    }

    token = lexer_next(lexer);
    if (token.type != T_PROGRAM) {
        parser_error(token, "Expecting 'Program'");
        return false;
    }

    token = lexer_next(lexer);
    if (token.type != T_BRACE_LEFT) {
        parser_error(token, "Expected {");
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
        parser_error(token, "Expected }");
        return false;
    }

    return true;
}

/**
 * <field_decl_list> -> <field_decl> <field_decl_list> | EPSILON
 */
bool parser_parse_field_decl_list(Lexer* lexer)
{
    // try to parse a field decl
    if (parser_parse_field_decl(lexer)) {
        // if that worked, we must have a field_decl_list
        if (!parser_parse_field_decl_list(lexer)) {
            return false;
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
    // try to parse a method decl
    if (parser_parse_method_decl(lexer)) {
        // if that worked, we must have a method_decl_list
        if (!parser_parse_method_decl_list(lexer)) {
            return false;
        }
    }

    // empty string works
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
    return parser_parse_id(lexer)
        && parser_parse_array_dim_decl(lexer)
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
            return false;
        }

        token = lexer_next(lexer);
        if (token.type != T_BRACKET_RIGHT) {
            parser_error(token, "Expected closing bracket for array declaration.");
            return false;
        }
    }

    // empty string works
    return true;
}

/**
 * <hex_digit_list> -> <hex_digit> <hex_digit_list> | EPSILON
 */
bool parser_parse_hex_digit_list(Lexer* lexer)
{
    //parser_parse_hex_digit(lexer);
    parser_parse_hex_digit_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <hex_literal> -> 0x <hex_digit> <hex_digit_list>
 */
bool parser_parse_hex_literal(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != "0x") {
        parser_error(token, "Expected 0x in hex_literal during parse and did not get it.");
    }

    //parser_parse_hex_digit(lexer);
    parser_parse_hex_digit_list(lexer);
}

/**
 * <digit_list> -> <digit> <digit_list> | EPSILON
 */
bool parser_parse_digit_list(Lexer* lexer)
{
    //parser_parse_digit(lexer);
    parser_parse_digit_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <decimal_literal> -> <digit> <digit_list>
 */
bool parser_parse_decimal_literal(Lexer* lexer)
{
    //parser_parse_digit(lexer);
    //parser_parse_digit_list(lexer);
}

/**
 * <cond_op> -> && OR ||
 */
bool parser_parse_cond_op(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != "&&" && token.lexeme != "||") {
        parser_error(token, "Expected && or || during parse and did not get them.");
    }
}

/**
 * <eq_op> -> == OR !=
 */
bool parser_parse_eq_op(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != "==" || token.lexeme != "!=") {
        parser_error(token, "Expected either == or != during the parse and did not get them.");
    }
}

/**
 * <rel_op> ->   < | > | <= | >=
 */
bool parser_parse_rel_op(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != "<" && token.lexeme != ">" && token.lexeme != "<=" && token.lexeme != ">=") {
        parser_error(token, "Expected any of the following and did not receive them during the parse: < > <= >=");
    }
}

/**
 * <arith_op> -> + | - | * | / | %
 */
bool parser_parse_arith_op(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != "+" && token.lexeme != "-" && token.lexeme != "*" && token.lexeme != "/" && token.lexeme != "%") {
        parser_error(token, "Expected any of the following and did not receive them during the parse: + - * / %");
    }
}

/**
 * <method_name> -> <id>
 */
bool parser_parse_method_name(Lexer* lexer)
{
    //parser_parse_id(lexer);
}

/**
 * <id> -> <alpha> <alpha_num_string>
 */
bool parser_parse_id(Lexer* lexer)
{
    //parser_parse_alpha(lexer);
    //parser_parse_alpha_num_string(lexer);
}

/**
 * <expr_end> -> <bin_op> <expr> | EPSILON
 */
bool parser_parse_expr_end(Lexer* lexer)
{
    //parser_parse_bin_op(lexer);
    // parser_parse_expr(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <location> -> <id> <array_subscript_expr>
 */
bool parser_parse_location(Lexer* lexer)
{
    //parser_parse_id(lexer);
    //parser_parse_array_subscript_expr(lexer);
}

/**
 * <expr_list_tail> -> , <expr> <expr_list_tail> | EPSILON
 */
bool parser_parse_expr_list_tail(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != ",") {
        parser_error(token, "Expected , when parsing expr_list_tail and didn't get it.");
    }

    //parser_parse_expr(lexer);
    //parser_parse_expr_list_tail(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <expr_list> -> <expr> <expr_list_tail> | EPSILON
 */
bool parser_parse_expr_list(Lexer* lexer)
{
    //parser_parse_expr(lexer);
    //parser_parse_expr_list_tail(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <callout_arg_list> -> , <callout_arg> <callout_arg_list> | EPSILON
 */
bool parser_parse_callout_arg_list(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != ",") {
        parser_error(token, "Expected comma when parsing callout_arg_list and didn't get it.");
    }

    //parser_parse_callout_arg(lexer);
    //parser_parse_callout_arg_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <assign_op> -> = | += | -=
 */
bool parser_parse_assign_op(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != "=" && token.lexeme != "+=" && token.lexeme != "-=") {
        parser_error(token, "Expected one of the following when parsing an assign_op and did not get them: = += -=");
    }
}

/**
 * <expr_option> -> <expr> | EPSILON
 */
bool parser_parse_expr_option(Lexer* lexer)
{
    //parser_parse_expr(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <else_expr> -> else <block> | EPSILON
 */
bool parser_parse_else_expr(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_ELSE) {
        parser_error(token, "Expected else while parsing else_expr and did not get it.");
    }

    //parser_parse_else_expr(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <var_decl> -> <type> <id> <var_id_list_tail>
 */
bool parser_parse_var_decl(Lexer* lexer)
{
    //parser_parse_type(lexer);
    //parser_parse_id(lexer);
    //parser_parse_var_id_list_tail(lexer);
}

/**
 * <statement_list> -> <statement> <statement_list> | EPSILON
 */
bool parser_parse_statement_list(Lexer* lexer)
{
    //parser_parse_statement(lexer);
    //parser_parse_statement_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <var_decl_list> -> <var_decl> <var_decl_list> | EPSILON
 */
bool parser_parse_var_decl_list(Lexer* lexer)
{
    //parser_parse_var_decl(lexer);
    //parser_parse_var_decl_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <block> -> { <var_decl_list> <statement_list> }
 */
bool parser_parse_block(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_BRACE_LEFT) {
        parser_error(token, "Expected left curly brace when parsing block and did not get one.");
    }

    //parser_parse_var_decl_list(lexer);
    //parser_parse_statement_list(lexer);

    token = lexer_next(lexer);
    if (token.type != T_BRACE_RIGHT) {
        parser_error(token, "Expected right curly brace when parsing block during parsing and did not get one.");
    }
}

/**
 * <method_param_decl> -> <type> <id>
 */
bool parser_parse_method_param_decl(Lexer* lexer)
{
    //parser_parse_type(lexer);
    //parser_parse_id(lexer);
}

/**
 * <method_param_decl_list_tail> -> , <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
bool parser_parse_method_param_decl_list_tail(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != ",") {
        parser_error(token, "Expected , when parsing method_param_decl_list_tail and didn't get one.");
    }

    //parser_parse_param_decl(lexer);
    //parser_parse_method_param_decl_list_tail(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <method_param_decl_list> -> <method_param_decl> <method_param_decl_list_tail> | EPSILON
 */
bool parser_parse_method_param_decl_list(Lexer* lexer)
{
    //parser_parse_method_param_decl(lexer);
    //parser_parse_method_param_decl_list_tail(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <field_id_list_tail> -> , <field_id_list> | ;
 */
bool parser_parse_field_id_list_tail(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type == T_COMMA) {
        //first derivation

        //parser_parse_field_id_list(lexer);
    } else if (token.type != T_STATEMENT_END) {
        parser_error(token, "Expected , or ; when parsing field_id_list_tail and got neither.");
    }
}

/**
 * <method_call> -> <method_name> ( <expr_list> )
 *                  | callout ( <string_literal> <callout_arg_list> )
 */
bool parser_parse_method_call(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type == T_CALLOUT) {
        //second derivation

        token = lexer_next(lexer);
        if (token.type != T_PAREN_LEFT) {
            parser_error(token, "Expected left parentheses when parsing method_call and did not get one.");
        }
        //parser_parse_string_literal(lexer);
        //parser_parse_callout_arg_list(lexer);

        token = lexer_next(lexer);
        if (token.type != T_PAREN_RIGHT) {
            parser_error(token, "Expected right parentheses when parsing method_call and did not get one.");
        }
    } else {
        //first derivation

        //parser_parse_method_name(lexer);
        token = lexer_next(lexer);
        if (token.type != T_PAREN_LEFT) {
            //do we have to move the lexer back now?
            parser_error(token, "Expected left parentheses when parsing method_call and didn't get one.");
        }
        //parser_parse_expr_list(lexer);

        token = lexer_next(lexer);
        if (token.type != T_PAREN_RIGHT) {
            //do we have to move the lexer back now?
            parser_error(token, "Expected right parentheses when parsing method_call and didn't get one.");
        }
    }
}

/**
 * <var_id_list_tail> -> , <id> <var_id_list_tail> | ;
 */
bool parser_parse_var_id_list_tail(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type == T_COMMA) {
        //first derivation

        //parser_parse_id(lexer);
        //parser_parse_var_id_list_tail(lexer);
    } else if (token.type != T_STATEMENT_END) {
        parser_error(token, "Expected a , or a ; when parsing var_id_list_tail and got neither.");
    }
}

/**
 * <method_decl> -> <type> <id> ( <method_param_decl_list> ) <block>
                    | void <id> ( <method_param_decl_list> ) <block>
 */
bool parser_parse_method_decl(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type == T_VOID) {
        //second derivation

        //parser_parse_id(lexer);
        token = lexer_next(lexer);
        if (token.type != T_PAREN_LEFT) {
            parser_error(token, "Expected left parentheses when parsing method_decl and did not get one.");
        }
        //parser_parse_method_param_decl_list(lexer);

        token = lexer_next(lexer);
        if (token.type != T_PAREN_RIGHT) {
            parser_error(token, "Expected right parentheses when parsing method_decl and did not get one.");
        }
        //parser_parse_block(lexer);
    } else {
        //first derivation

        //parser_parse_type(lexer);
        //parser_parse_id(lexer);
        token = lexer_next(lexer);
        if (token.type != T_PAREN_LEFT) {
            parser_error(token, "Expected left parentheses when parsing method_decl and did not get one.");
        }
        //parser_parse_method_param_decl_list(lexer);
        token = lexer_next(lexer);
        if (token.type != T_PAREN_RIGHT) {
            parser_error(token, "Expected right parentheses when parsing method_decl and did not get one.");
        }
        //parser_parse_block(lexer);
    }
}

/**
 * <alpha_num_string> -> <alpha_num> <alpha_num_string> | EPSILON
 */
bool parser_parse_alpha_num_string(Lexer* lexer)
{
    //parser_parse_alpha_num(lexer);
    //parser_parse_alpha_num_string(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <array_subscript_expr> -> [ <expr> ] | EPSILON
 */
bool parser_parse_array_subscript_expr(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_BRACKET_RIGHT) {
        parser_error(token, "Expected a right bracket when parsing array_subscript_expr but didn't get one.");
    }
    //parser_parse_expr(lexer);
    token = lexer_next(lexer);
    if (token.type != T_BRACKET_LEFT) {
        parser_error(token, "Expected a left bracket when parsing array_subscript_expr but didn't get one.");
    }

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <type> -> int | boolean
 */
bool parser_parse_type(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_BOOLEAN && token.type != T_INT) {
        return parser_error(token, "Expected int or boolean during parse and did not get them.");
    }
}

/**
 * <int_literal> -> <decimal_literal> | <hex_literal>
 */
bool parser_parse_int_literal(Lexer* lexer)
{
    // todo
    return false;
}

/**
 * <bool_literal> -> true | false
 */
bool parser_parse_bool_literal(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.type != T_BOOLEAN_LITERAL) {
        parser_error(token, "Expected boolean literal during parse and did not get it.");
        return false;
    }

    return true;
}

/**
 * <char_literal> -> ’ <char> ’
 */
bool parser_parse_char_literal(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != "'") {
        parser_error(token, "Expected ' in char_literal during parse and did not get it.");
    }

    //parser_parse_char(lexer);

    token = lexer_next(lexer);
    if (token.lexeme != "'") {
        parser_error(token, "Expected ' in char_literal during parse and did not get it.");
    }
}

/**
 * <string_literal> -> " <char> "
 */
bool parser_parse_string_literal(Lexer* lexer)
{
    Token token = lexer_next(lexer);
    if (token.lexeme != "\"") {
        parser_error(token, "Expected \" in char_literal during parse and did not get it.");
    }

    //parser_parse_char(lexer);

    token = lexer_next(lexer);
    if (token.lexeme != "\"") {
        parser_error(token, "Expected \" in char_literal during parse and did not get it.");
    }
}
