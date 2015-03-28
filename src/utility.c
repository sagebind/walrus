#include <stdbool.h>
#include <stdlib.h>
#include "utility.h"

#define KEYWORD_COUNT 14

/**
 * An array of all reserved keywords.
 */
char* keywords[KEYWORD_COUNT] = {
    "boolean", "break", "callout", "class", "continue", "else", "false", "for",
    "if", "int", "Program", "return", "true", "void"
};

/**
 * An array of token types that correspond to a keyword by index.
 */
TokenType keyword_tokens[KEYWORD_COUNT] = {
    T_BOOLEAN, T_BREAK, T_CALLOUT, T_CLASS, T_CONTINUE, T_ELSE, T_BOOLEAN_LITERAL,
    T_FOR, T_IF, T_INT, T_PROGRAM, T_RETURN, T_BOOLEAN_LITERAL, T_VOID
};

/**
 * Checks to see if the input string is a reserved keyword.
 */
bool is_keyword(char* inString)
{
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (strcmp(inString, keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * Creates a token based on the passed in keyword.
 */
Token create_keyword_token(char* in_keyword, ScannerContext* context)
{
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (strcmp(in_keyword, keywords[i]) == 0) {
            return token_create(context->line, context->column, keyword_tokens[i], in_keyword);
        }
    }
    return token_create(context->line, context->column, T_ILLEGAL, in_keyword);
}
