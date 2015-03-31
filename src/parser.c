#include <stdlib.h>
#include "error.h"
#include "lexer.h"
#include "parser.h"


/**
 * Parses the tokens yielded by a given lexer.
 */
ParseTreeNode* parser_parse(Lexer* lexer)
{
    // create the root node
    ParseTreeNode* root = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    return root;
}

/**
 * <program> -> class Program { 〈field_decl_list〉 〈method_decl_list〉 }
 */
ParseTreeNode* parser_parse_program(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if (t.type != T_CLASS) {
        error(E_LEXER_ERROR, "A program starts with 'class', you fool.");
    }

    t = lexer_next(lexer);
    if (t.type != T_PROGRAM) {
        error(E_LEXER_ERROR, "");
    }

    t = lexer_next(lexer);
    if (t.type != T_BRACE_LEFT) {
        error(E_LEXER_ERROR, "");
    }

    //parser_parse_field_decl_list(lexer);
    //parser_parse_method_decl_list(lexer);

    t = lexer_next(lexer);
    if (t.type != T_BRACE_RIGHT) {
        error(E_LEXER_ERROR, "");
    }
}

/**
 * <bool_literal> -> true | false
 */
ParseTreeNode* parser_parse_bool_literal(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.type != T_BOOLEAN_LITERAL) {
        error(E_LEXER_ERROR, "Expected boolean literal during parse and did not get it.");
    }
}

/**
 * <char_literal> -> ’ 〈char〉 ’
 */
ParseTreeNode* parser_parse_char_literal(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != "'") {
        error(E_LEXER_ERROR, "Expected ' in char_literal during parse and did not get it.");
    }

    //parser_parse_char(lexer);

    t = lexer_next(lexer);
    if(t.lexeme != "'") {
        error(E_LEXER_ERROR, "Expected ' in char_literal during parse and did not get it.");
    }
}

/**
 * <string_literal> -> " 〈char〉 "
 */
ParseTreeNode* parser_parse_string_literal(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != "\"") {
        error(E_LEXER_ERROR, "Expected \" in char_literal during parse and did not get it.");
    }

    //parser_parse_char(lexer);

    t = lexer_next(lexer);
    if(t.lexeme != "\"") {
        error(E_LEXER_ERROR, "Expected \" in char_literal during parse and did not get it.");
    }
}

/**
 * <hex_digit_list> -> 〈hex_digit〉 〈hex_digit_list〉 | EPSILON
 */
ParseTreeNode* parser_parse_hex_digit_list(Lexer* lexer)
{
    //parser_parse_hex_digit(lexer);
    parser_parse_hex_digit_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <hex_literal> -> 0x 〈hex_digit〉 〈hex_digit_list〉
 */
ParseTreeNode* parser_parse_hex_literal(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != "0x") {
        error(E_LEXER_ERROR, "Expected 0x in hex_literal during parse and did not get it.");
    }

    //parser_parse_hex_digit(lexer);
    parser_parse_hex_digit_list(lexer);
}

/**
 * <digit_list> -> 〈digit〉 〈digit_list〉 | EPSILON
 */
ParseTreeNode* parser_parse_digit_list(Lexer* lexer)
{
    //parser_parse_digit(lexer);
    parser_parse_digit_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <decimal_literal> -> 〈digit〉 〈digit_list〉
 */
ParseTreeNode* parser_parse_decimal_literal(Lexer* lexer)
{
    //parser_parse_digit(lexer);
    //parser_parse_digit_list(lexer);
}

/**
 * <cond_op> -> && OR ||
 */
ParseTreeNode* parser_parse_cond_op(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != "&&" && t.lexeme != "||") {
        error(E_LEXER_ERROR, "Expected && or || during parse and did not get them.");
    }
}

/**
 * <eq_op> -> == OR !=
 */
ParseTreeNode* parser_parse_eq_op(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != "==" || t.lexeme != "!=") {
        error(E_LEXER_ERROR, "Expected either == or != during the parse and did not get them.");
    }
}

/**
 * <rel_op> ->   < | > | <= | >=
 */
ParseTreeNode* parser_parse_rel_op(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != "<" && t.lexeme != ">" && t.lexeme != "<=" && t.lexeme != ">=") {
        error(E_LEXER_ERROR, "Expected any of the following and did not receive them during the parse: < > <= >=");
    }
}

/**
 * <arith_op> -> + | - | * | / | %
 */
ParseTreeNode* parser_parse_arith_op(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != "+" && t.lexeme != "-" && t.lexeme != "*" && t.lexeme != "/" && t.lexeme != "%") {
        error(E_LEXER_ERROR, "Expected any of the following and did not receive them during the parse: + - * / %");
    }
}

/**
 * <method_name> -> <id>
 */
ParseTreeNode* parser_parse_method_name(Lexer* lexer)
{
    //parser_parse_id(lexer);
}

/**
 * <id> -> 〈alpha〉 〈alpha_num_string〉
 */
ParseTreeNode* parser_parse_id(Lexer* lexer)
{
    //parser_parse_alpha(lexer);
    //parser_parse_alpha_num_string(lexer);
}

/**
 * <expr_end> -> 〈bin_op〉 〈expr〉 
 */
ParseTreeNode* parser_parse_expr_end(Lexer* lexer)
{
    //parser_parse_bin_op(lexer);
    // parser_parse_expr(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <location> -> 〈id〉 〈array_subscript_expr〉
 */
ParseTreeNode* parser_parse_location(Lexer* lexer)
{
    //parser_parse_id(lexer);
    //parser_prase_array_subscript_expr(lexer);
}

/**
 * <expr_list_tail> -> , 〈expr〉 〈expr_list_tail〉
 */
ParseTreeNode* parser_parse_expr_list_tail(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != ",") {
        error(E_LEXER_ERROR, "Expected , when parsing expr_list_tail and didn't get it.");
    }

    //parser_parse_expr(lexer);
    //parser_parse_expr_list_tail(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <expr_list> -> 〈expr〉 〈expr_list_tail〉
 */
ParseTreeNode* parser_parse_expr_list(Lexer* lexer)
{
    //parser_parse_expr(lexer);
    //parser_parse_expr_list_tail(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <callout_arg_list> -> , 〈callout_arg〉 〈callout_arg_list〉
 */
ParseTreeNode* parser_parse_callout_arg_list(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != ",") {
        error(E_LEXER_ERROR, "Expected comma when parsing callout_arg_list and didn't get it.");
    }

    //parser_parse_callout_arg(lexer);
    //parser_parse_callout_arg_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <assign_op> -> = | += | -=
 */
ParseTreeNode* parser_parse_assign_op(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.lexeme != "=" && t.lexeme != "+=" && t.lexeme != "-=") {
        error(E_LEXER_ERROR, "Expected one of the following when parsing an assign_op and did not get them: = += -=");
    }
}

/**
 * <expr_option> -> 〈expr〉 | EPSILON
 */
ParseTreeNode* parser_parse_expr_option(Lexer* lexer)
{
    //parser_parse_expr(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <else_expr> -> else 〈block〉 | EPSILON
 */
ParseTreeNode* parser_parse_else_expr(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.type != T_ELSE) {
        error(E_LEXER_ERROR, "Expected else while parsing else_expr and did not get it.");
    }

    //parser_parse_else_expr(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <var_decl> -> 〈type〉 〈id〉 〈var_id_list_tail〉
 */
ParseTreeNode* parser_parse_var_decl(Lexer* lexer)
{
    //parser_parse_type(lexer);
    //parser_parse_id(lexer);
    //parser_parse_var_id_list_tail(lexer);
}

/**
 * <statement_list> -> 〈statement〉 〈statement_list〉
 */
ParseTreeNode* parser_parse_statement_list(Lexer* lexer)
{
    //parser_parse_statement(lexer);
    //parser_parse_statement_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <var_decl_list> -> 〈var_decl〉 〈var_decl_list〉
 */
ParseTreeNode* parser_parse_var_decl_list(Lexer* lexer)
{
    //parser_parse_var_decl(lexer);
    //parser_parse_var_decl_list(lexer);

    //HANDLE ALTERNATE EPSILON DERIVATION PLZ - 0--}--{
}

/**
 * <block> -> { 〈var_decl_list〉 〈statement_list〉 }
 */
ParseTreeNode* parser_parse_block(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if(t.type != T_BRACE_LEFT) {
        error(E_LEXER_ERROR, "Expected left curly brace when parsing block and did not get one.");
    }

    //parser_parse_var_decl_list(lexer);
    //parser_parse_statement_list(lexer);

    t = lexer_next(lexer);
    if(t.type != T_BRACE_RIGHT) {
        error(E_LEXER_ERROR, "Expected right curly brace when parsing block during parsing and did not get one.");
    }
}

/**
 * <method_param_decl> -> 〈type〉 〈id〉
 */
ParseTreeNode* parser_parse_method_param_decl(Lexer* lexer)
{
    //parser_parse_type(lexer);
    //parser_parse_id(lexer);
}

/**
 * <type> -> int | boolean
 */
ParseTreeNode* parser_parse_type(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if (t.type != T_BOOLEAN && t.type != T_INT) {
        error(E_LEXER_ERROR, "Expected int or boolean during parse and did not get them.");
    }
}
