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
 * <type> -> int | boolean
 */
ParseTreeNode* parser_parse_type(Lexer* lexer)
{
    Token t = lexer_next(lexer);
    if (t.type != T_BOOLEAN && t.type != T_INT) {
        error(E_LEXER_ERROR, "Expected int or boolean during parse and did not get it.");
    }
}
