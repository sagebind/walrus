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
 * <type> -> int | boolean
 */
ParseTreeNode* parser_parse_type(Lexer* lexer)
{
    Token t = lexer_next(lexer);

    if (t.type != T_BOOLEAN && t.type != T_INT) {
        error(E_LEXER_ERROR, "");
    }
}
