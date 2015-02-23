#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "tokens.h"

/**
 * Parses the next token from a scanner context.
 */
Token lexer_next(ScannerContext* context)
{
    // get the next char from the scanner
    char character = scanner_next(context);

    // skip whitespace characters
    while (isspace(character)) {
        character = scanner_next(context);
    }

    // branch into attempts at matching different token types
    switch (character) {
        // end of file
        case EOF:
            return token_create(context->line, context->column, T_EOF, " ");

        // single-character operators ;)
        case '*': case '/': case '%':
            return token_create(
                context->line,
                context->column,
                T_OPERATOR,
                scanner_get_string(context, -1) // get the string from position-1 to position
            );

        // + +=
        case '+':
            //check next token to see if it's an equal sign
            if(scanner_peek(context, 0) == '=') {
                scanner_advance(context, 1);
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "+="
                );
            } else {
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "+"
                );
            }

        // - -=
        case '-':
            //check next token to see if it's an equal sign
            if(scanner_peek(context, 0) == '=') {
                scanner_advance(context, 1);
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "-="
                );
            } else {
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "-"
                );
            }

        // = ==
        case '=':
            //check next token to see if it's an equal sign
            if(scanner_peek(context, 0) == '=') {
                scanner_advance(context, 1);
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "=="
                );
            } else {
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "="
                );
            }

        // looks like the beginning of a char
        case '\'':
			if (scanner_peek(context, 0) != '\'' && scanner_peek(context, 1) == '\'') {
                scanner_advance(context, 2);
                return token_create(
                    context->line,
                    context->column,
                    T_CHAR_LITERAL,
                    scanner_get_string(context, -3)
                );
            }
            break;

        // looks like the beginning of a string
        case '"':
            //return lexer_lex_string(context); @todo
			if(scanner_peek(context, 0) == '*') {
                scanner_advance(context, 1);
                return token_create(
                    context->line,
                    context->column,
                    T_STRING_LITERAL,
                    "\""
                );
            }
            break;

        // > >=
        case '>':
            //check next token to see if it's an equal sign
            if(scanner_peek(context, 0) == '=') {
                scanner_advance(context, 1);
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    ">="
                );
            } else {
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    ">"
                );
            }

        // < <=
        case '<':
            //check next token to see if it's an equal sign
            if(scanner_peek(context, 0) == '=') {
                scanner_advance(context, 1);
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "<="
                );
            } else {
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "<"
                );
            }

        // &&
        case '&':
            if(scanner_peek(context, 0) == '&') {
                scanner_advance(context, 1);
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "&&"
                );
            } else {
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "&"
               );
            }

        // ||
        case '|':
            if(scanner_peek(context, 0) == '|') {
                scanner_advance(context, 1);
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "||"
                );
            } else {
                return token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    "|"
               );
            }

        // nothing matched so far, try variable matching
        default:
            // looks like the start of an identifier
            if (isalpha(character) || character == '_') {
                // todo
            }
    }

    // we tried everything, lets call it a day
    return token_create(
        context->line,
        context->column,
        T_ILLEGAL,
        scanner_get_string(context, -1)
    );
}

/**
 * Prints tokens to standard output.
 */
void lexer_print_tokens(TokenStream* tokens)
{
    // loop over each token in the stream
    for (int i = 0; i < tokens->length; ++i) {
        printf("%d ", tokens->tokens[i].line);

        if (tokens->tokens[i].type == T_BOOLEAN_LITERAL) {
            printf("BOOLEANLITERAL ");
        } else if (tokens->tokens[i].type == T_CHAR_LITERAL) {
            printf("CHARLITERAL ");
        } else if (tokens->tokens[i].type == T_INT_LITERAL) {
            printf("INTLITERAL ");
        } else if (tokens->tokens[i].type == T_STRING_LITERAL) {
            printf("STRINGLITERAL ");
        } else if (tokens->tokens[i].type == T_ILLEGAL) {
            printf("ILLEGAL ");
        }

        if (tokens->tokens[i].type != T_EOF) {
            printf("%s\r\n", tokens->tokens[i].lexeme);
        }
    }
}
