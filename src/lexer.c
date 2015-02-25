#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "tokens.h"
#include "utility.h"

/**
 * Tokenizes source code read from a scanner context.
 */
TokenStream* lexer_tokenize(ScannerContext* context)
{
    TokenStream* tokens = token_stream_create(1);

    Token token;
    while (token.type != T_EOF) {
        token = lexer_next(context);
        token_stream_push(tokens, token);

        if (token.type != T_EOF) {
            lexer_print_token(token);
        }
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

            // ! !=
            case '!':
                //check next token to see if it's an equal sign
                if(scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "!="
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
                token = lexer_lex_char(context);
                break;

            // looks like the beginning of a string
            case '\"':
                token = lexer_lex_string(context);
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
				
			            // ;
            case ';':
              
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        ";"
                break;
				
						            // ;
            case '{':
              
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        T_OPERATOR,
                        "{"
                break;


            // nothing matched so far, try variable matching
            default:
                // looks like the start of an identifier
                if (isalpha(character) || character == '_') {
                    token = lexer_lex_identifier(context);
                    break;
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
 * Reads an identifier token in the current context.
 */
Token lexer_lex_identifier(ScannerContext* context)
{
    char* identifier;
    identifier[0] = scanner_peek(context, -1);
    char secondChar = scanner_peek(context, 0);

    if(isspace(secondChar) || secondChar == ';') {
        identifier[1] = '\0';
        //valid identifier - create token
        return token_create(
            context->line,
            context->column,
            T_IDENTIFIER,
            identifier
        );
    } else {
        scanner_advance(context, 1);
        identifier[1] = secondChar;
        //start counter at 2 since we already have the first and second positions of identifier filled
        int counter = 2;
        while (!context->eol && !context->eof) {
            char nextChar = scanner_peek(context, 0);
            if(isspace(nextChar) || nextChar == ';') {
                //end of identifier - check it against reserved keywords and then create token
                identifier[counter] = '\0';
                if(!isReserved(identifier)) {
                    //valid identifier - create token
                    return token_create(
                        context->line,
                        context->column,
                        T_IDENTIFIER,
                        identifier
                    );
                } else {
                    lexer_error("Illegal name for identifier - same name as a reserved keyword", context);
                    return token_create(
                        context->line,
                        context->column,
                        T_ILLEGAL,
                        identifier
                    );
                }
            } else {
                //current char is non-terminating and valid, continue building identifier
                scanner_advance(context, 1);
                identifier[counter] = scanner_next(context);
                counter++;
            }
        }
    }
}

/**
 * Reads a char token in the current context.
 */
Token lexer_lex_char(ScannerContext* context)
{
    // get the actual character in the char literal
    char character = scanner_next(context);

    // char is escaped
    bool is_escaped = false;
    if (character == '\\') {
        character = lexer_scan_escaped(context);
        is_escaped = true;
    }

    // invalid character
    else if (character < 32 || character > 126 || character == '\'' || character == '"') {
        lexer_error("Illegal character in char literal", context);
        return token_create(
            context->line,
            context->column,
            T_ILLEGAL,
            scanner_get_string(context, -2)
        );
    }

    // char literal must close here
    if (scanner_next(context) != '\'') {
        lexer_error("Char literal must contain only one character", context);
        return token_create(
            context->line,
            context->column,
            T_ILLEGAL,
            scanner_get_string(context, -2)
        );
    }

    // if we made it this far, the char literal must be valid!
    return token_create(
        context->line,
        context->column,
        T_CHAR_LITERAL,
        scanner_get_string(context, is_escaped ? -4 : -3)
    );
}

/**
 * Reads a string token in the current context.
 */
Token lexer_lex_string(ScannerContext* context)
{
    char character;
    int length = 0; // length of string

    // consumes next char until end of string or file
    while (!context->eof) {
        character = scanner_next(context);

        // end of string?
        if (character == '"') {
            break;
        }

        length++;

        // char is escaped
        if (character == '\\') {
            character = lexer_scan_escaped(context);
            length++;
        }

        // invalid character
        else if (character < 32 || character > 126 || character == '\'') {
            lexer_error("Illegal character in string literal", context);
            return token_create(
                context->line,
                context->column,
                T_ILLEGAL,
                scanner_get_string(context, -1 - length)
            );
        }

    }

    // we made it this far; must be OK
    return token_create(
        context->line,
        context->column,
        T_STRING_LITERAL,
        scanner_get_string(context, -2 - length)
    );
}

/**
 * Scans an escaped character and returns the actual character, or -1 if the char is invalid.
 */
char lexer_scan_escaped(ScannerContext* context)
{
    char escape_modifier = scanner_next(context);

    switch (escape_modifier) {
        case '"':
            return '"';
        case '\'':
            return '\'';
        case '\\':
            return '\\';
        case 't':
            return '\t';
        case 'n':
            return '\n';
    }

    return -1;
}

/**
 * Prints a token to standard output.
 */
void lexer_print_token(Token token)
{
    printf("%d ", token.line);

    if (token.type == T_BOOLEAN_LITERAL) {
        printf("BOOLEANLITERAL ");
    } else if (token.type == T_CHAR_LITERAL) {
        printf("CHARLITERAL ");
    } else if (token.type == T_INT_LITERAL) {
        printf("INTLITERAL ");
    } else if (token.type == T_STRING_LITERAL) {
        printf("STRINGLITERAL ");
    } else if (token.type == T_IDENTIFIER) {
        printf("IDENTIFIER ");
    } else if (token.type == T_ILLEGAL) {
        printf("ILLEGAL ");
    }

    printf("%s\n", token.lexeme);
}

/**
 * Displays an error message for syntax errors.
 */
void lexer_error(char* message, ScannerContext* context)
{
    printf("%s line %d:%d: %s\n", context->name, context->line, context->column, message);
}
