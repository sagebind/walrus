#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "tokens.h"

/**
 * Tokenizes source code read from a scanner context.
 */
TokenStream* lexer_tokenize(ScannerContext* context)
{
    TokenStream* tokens = token_stream_create(1);
    Token token;
    while (token.type != T_EOF) {
        token = lexer_next(context);

        if (token.type == T_ILLEGAL) {
            fprintf(stderr, "Error: Illegal token at line %d, column %d: %s\n", token.line, token.column, token.lexeme);
            exit(1);
        }

        token_stream_push(tokens, token);
    }

    return tokens;
}

/**
 * Parses the next token from a scanner context.
 */
Token lexer_next(ScannerContext* context)
{
    // the current matching token
    Token token;
    char character;

    // attempt to match a token until a non-whitespace token is found
    do {
        // get the next char from the scanner
        character = scanner_next(context);

        // try to match normal whitespace and skip it
        if (isspace(character)) {
            token = token_create(context->line, context->column, T_WHITESPACE, " ");
            continue;
        }

        // branch into attempts at matching different token types
        switch (character) {
            // end of file
            case EOF:
                token = token_create(context->line, context->column, T_EOF, " ");
                break;

            // / //
            case '/':
                // comment line?
                if (scanner_peek(context, 0) == '/') {
                    while (!context->eol && !context->eof) {
                        scanner_next(context);
                    }
                    token = token_create(context->line, context->column, T_WHITESPACE, " ");
                } else {
                    token = token_create(context->line, context->column, T_OPERATOR, "/");
                }
                break;

            // single-character operators ;)
            case '*': case '%':
                token = token_create(
                    context->line,
                    context->column,
                    T_OPERATOR,
                    scanner_get_string(context, -1) // get the string from position-1 to position
                );
                break;

            // + +=
            case '+':
                //check next token to see if it's an equal sign
                if(scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "+="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "+"
                    );
                }
                break;

            // - -=
            case '-':
                //check next token to see if it's an equal sign
                if(scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "-="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "-"
                    );
                }
                break;

            // = ==
            case '=':
                //check next token to see if it's an equal sign
                if(scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "=="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "="
                    );
                }
                break;

            // looks like the beginning of a char
            case '\'':
    			if (scanner_peek(context, 0) != '\'' && scanner_peek(context, 1) == '\'') {
                    scanner_advance(context, 2);
                    token = token_create(
                        context->line,
                        context->column,
                        T_CHAR_LITERAL,
                        scanner_get_string(context, -3)
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_ILLEGAL,
                        scanner_get_string(context, -3)
                    );
                }
                break;

            // looks like the beginning of a string
            case '"':
                //return lexer_lex_string(context); @todo
    			if(scanner_peek(context, 0) == '*') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_STRING_LITERAL,
                        "\""
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_ILLEGAL,
                        scanner_get_string(context, -1)
                    );
                }
                break;

            // > >=
            case '>':
                //check next token to see if it's an equal sign
                if(scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        ">="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        ">"
                    );
                }
                break;

            // < <=
            case '<':
                //check next token to see if it's an equal sign
                if(scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "<="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "<"
                    );
                }
                break;

            // &&
            case '&':
                if(scanner_peek(context, 0) == '&') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "&&"
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "&"
                   );
                }
                break;

            // ||
            case '|':
                if(scanner_peek(context, 0) == '|') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "||"
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "|"
                   );
                }
                break;

            // nothing matched so far, try variable matching
            default:
                // looks like the start of an identifier
                if (isalpha(character) || character == '_') {
                    // todo
                }

                // we tried everything, lets call it a day
                token = token_create(
                    context->line,
                    context->column,
                    T_ILLEGAL,
                    scanner_get_string(context, -1)
                );
        }
    } while (token.type == T_WHITESPACE && !context->eof);

    return token;
}

/**
 * Prints tokens to standard output.
 */
void lexer_print_tokens(TokenStream* tokens)
{
    // loop over each token in the stream
    for (int i = 0; i < tokens->length; ++i) {
        if (tokens->tokens[i].type == T_EOF) {
            continue;
        }

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

        printf("%s\n", tokens->tokens[i].lexeme);
    }
}
