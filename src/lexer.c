#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "tokens.h"

#define KEYWORD_COUNT 14


/**
 * An array of all reserved keywords.
 */
static char* keyword_identifiers[KEYWORD_COUNT] = {
    "boolean", "break", "callout", "class", "continue", "else", "false", "for",
    "if", "int", "Program", "return", "true", "void"
};

/**
 * An array of token types that correspond to a keyword by index.
 */
static TokenType keyword_token_types[KEYWORD_COUNT] = {
    T_BOOLEAN, T_BREAK, T_CALLOUT, T_CLASS, T_CONTINUE, T_ELSE, T_BOOLEAN_LITERAL,
    T_FOR, T_IF, T_INT, T_PROGRAM, T_RETURN, T_BOOLEAN_LITERAL, T_VOID
};


/**
 * Creates a new stateful lexer instance.
 */
Lexer* lexer_create(ScannerContext* context)
{
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->context = context;
    lexer->tokens = token_stream_create();
    lexer->current_node = NULL;
    return lexer;
}

/**
 * Gets the next token from a lexer and advances forward one token.
 */
Token lexer_next(Lexer* lexer)
{
    // check if we are at the tail of the stream
    if (lexer->current_node == lexer->tokens->tail) {
        // only read another token if we haven't reached the end-of-file
        if (lexer->current_node == NULL || lexer->current_node->token.type != T_EOF) {
            // read another token
            Token token = lexer_read_token(lexer->context);
            token_stream_push(lexer->tokens, token);
            lexer->current_node = lexer->tokens->tail;
        }
    } else {
        // we aren't at the tail, so move forward in the stream
        lexer->current_node = lexer->current_node->next;
    }

    // return the current token
    return lexer->current_node->token;
}

/**
 * Backtracks a lexer by one token.
 */
Error lexer_backtrack(Lexer* lexer)
{
    // make sure pointer isn't null
    if (lexer == NULL) {
        return E_BAD_POINTER;
    }

    // can't backtrack if no tokens have been read or we are at the beginning
    if (lexer->current_node == NULL || lexer->current_node->previous == NULL) {
        return E_OPERATION_FAILED;
    }

    // move backwards
    lexer->current_node = lexer->current_node->previous;
    return E_SUCCESS;
}

/**
 * Looks ahead a given number of tokens.
 */
Token lexer_lookahead(Lexer* lexer, int count)
{
    // advance by the given count and capture the token
    Token token;
    for (int i = 0; i < count; i++) {
        token = lexer_next(lexer);

        // handle lookahead over the end-of-file
        if (token.type == T_EOF) {
            count = i + 1;
            break;
        }
    }

    // retreat back to previous token
    for (int i = 0; i < count; i++) {
        lexer_backtrack(lexer);
    }

    return token;
}

/**
 * Destroys an open lexer.
 */
Error lexer_destroy(Lexer** lexer)
{
    // make sure pointer isn't null
    if (lexer == NULL) {
        return E_BAD_POINTER;
    }

    // destroy the token stream
    token_stream_destroy(&((**lexer).tokens)); // dereference lexer twice, access
                                               // tokens, and get its address

    // free memory for lexer
    free(*lexer);
    *lexer = NULL;

    return E_SUCCESS;
}

/**
 * Reads the next token from a scanner context.
 */
Token lexer_read_token(ScannerContext* context)
{
    // the current matching token
    Token token;
    char character;

    // attempt to match a token until a non-whitespace token is found
    do {
        // get the next char from the scanner
        character = scanner_next(context);

        // branch into attempts at matching different token types
        switch (character) {
            // end of file
            case EOF:
                token = token_create(context->line, context->column, context->name, T_EOF, "EOF");
                break;

            // simple single-character tokens
            case ';':
                token = token_create(context->line, context->column, context->name, T_STATEMENT_END, ";");
                break;

            case ',':
                token = token_create(context->line, context->column, context->name, T_COMMA, ",");
                break;

            case '{':
                token = token_create(context->line, context->column, context->name, T_BRACE_LEFT, "{");
                break;

            case '}':
                token = token_create(context->line, context->column, context->name, T_BRACE_RIGHT, "}");
                break;

            case '[':
                token = token_create(context->line, context->column, context->name, T_BRACKET_LEFT, "[");
                break;

            case ']':
                token = token_create(context->line, context->column, context->name, T_BRACKET_RIGHT, "]");
                break;

            case '(':
                token = token_create(context->line, context->column, context->name, T_PAREN_LEFT, "(");
                break;

            case ')':
                token = token_create(context->line, context->column, context->name, T_PAREN_RIGHT, ")");
                break;

            // / //
            case '/':
                // comment line?
                if (scanner_peek(context, 0) == '/') {
                    while (!context->eol && !context->eof) {
                        scanner_next(context);
                    }
                    token = token_create(context->line, context->column, context->name, T_WHITESPACE, " ");
                } else {
                    token = token_create(context->line, context->column, context->name, T_OPERATOR, "/");
                }
                break;

            // single-character operators ;)
            case '*': case '%':
                token = token_create(
                    context->line,
                    context->column,
                    context->name,
                    T_OPERATOR,
                    scanner_get_string(context, -1) // get the string from position-1 to position
                );
                break;

            // + +=
            case '+':
                // check next token to see if it's an equal sign
                if (scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "+="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "+"
                    );
                }
                break;

            // - -=
            case '-':
                // check next token to see if it's an equal sign
                if (scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "-="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "-"
                    );
                }
                break;

            // = ==
            case '=':
                // check next token to see if it's an equal sign
                if (scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "=="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "="
                    );
                }
                break;

            // ! !=
            case '!':
                // check next token to see if it's an equal sign
                if (scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "!="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "!"
                    );
                }
                break;

            // > >=
            case '>':
                // check next token to see if it's an equal sign
                if (scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        ">="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        ">"
                    );
                }
                break;

            // < <=
            case '<':
                // check next token to see if it's an equal sign
                if (scanner_peek(context, 0) == '=') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "<="
                    );
                } else {
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "<"
                    );
                }
                break;

            // &&
            case '&':
                if (scanner_peek(context, 0) == '&') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "&&"
                    );
                } else { // nothing else starts with &
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_ILLEGAL,
                        "&"
                   );
                }
                break;

            // ||
            case '|':
                if (scanner_peek(context, 0) == '|') {
                    scanner_advance(context, 1);
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_OPERATOR,
                        "||"
                    );
                } else { // nothing else starts with |
                    token = token_create(
                        context->line,
                        context->column,
                        context->name,
                        T_ILLEGAL,
                        "|"
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

            // nothing matched so far, try variable matching
            default:
                // try to match normal whitespace to skip it
                if (isspace(character) && character != 0xC) {
                    token = token_create(context->line, context->column, context->name, T_WHITESPACE, " ");
                    break;
                }

                // looks like the start of an identifier
                if (isalpha(character) || character == '_') {
                    token = lexer_lex_identifier(context);
                    break;
                }

                // start of an int literal
                if (isdigit(character)) {
                    token = lexer_lex_int(context);
                    break;
                }

                // we tried everything, lets call it a day
                lexer_error(context, character, -1);
                token = token_create(
                    context->line,
                    context->column,
                    context->name,
                    T_ILLEGAL,
                    scanner_get_string(context, -1)
                );
        }
    } while (token.type == T_WHITESPACE);

    return token;
}

/**
 * Reads an identifier token in the current context.
 */
Token lexer_lex_identifier(ScannerContext* context)
{
    // continuously peek then advance by one until a non-alphanumeric character is found
    for (int length = 1; !context->eof; length++) {
        // peek ahead at the next char
        char c = scanner_peek(context, 0);

        // is the next character the end of the identifier?
        if (!isalnum(c) && c != '_') {
            // get the entire identifier string
            char* identifier = scanner_get_string(context, 0 - length);

            // check if the identifier is a keyword
            if (lexer_identifier_is_keyword(identifier)) {
                // return a keyword token instead
                return lexer_create_keyword_token(identifier, context);
            }

            // valid, non keyword identifier - create a token
            return token_create(
                context->line,
                context->column,
                context->name,
                T_IDENTIFIER,
                identifier
            );
        }

        // consume the peeked char
        scanner_advance(context, 1);
    }
}

/**
 * Reads an int literal token in the current context.
 */
Token lexer_lex_int(ScannerContext* context)
{
    bool hexadecimal = false;
    int length = 1; // we already consumed the first digit

    // is it the start of a hex literal?
    if (scanner_peek(context, 0) == 'x' && scanner_peek(context, -1) == '0') {
        hexadecimal = true;
        scanner_advance(context, 1);
        length++;
    }

    // consume chars until we hit a non-int character
    while (true) {
        // peek ahead
        char c = scanner_peek(context, 0);

        // is it a digit?
        if (!isdigit(c)) {
            // it's OK if it is hexadecimal
            if (!hexadecimal || strchr("ABCDEFabcdef", c) == NULL) {
                // nvm, not even OK for hexadecimal
                break;
            }
        }

        // move ahead 1 char
        scanner_advance(context, 1);
        length++;
    }

    // check to make sure that a hex string is at least 3 characters long
    if (hexadecimal && length <= 2) {
        lexer_error(context, scanner_next(context), -1);

        return token_create(
            context->line,
            context->column,
            context->name,
            T_ILLEGAL,
            scanner_get_string(context, 0 - length)
        );
    }

    // we made it! return the token
    return token_create(
        context->line,
        context->column,
        context->name,
        T_INT_LITERAL,
        scanner_get_string(context, 0 - length)
    );
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
        is_escaped = true;
        character = lexer_scan_escaped(context);

        // was it a valid escape char?
        if (character == -1) {
            return token_create(
                context->line,
                context->column,
                context->name,
                T_ILLEGAL,
                scanner_get_string(context, -2)
            );
        }
    }

    // invalid character
    else if (character < 32 || character > 126 || character == '\'' || character == '"') {
        lexer_error(context, character, -1);
        return token_create(
            context->line,
            context->column,
            context->name,
            T_ILLEGAL,
            scanner_get_string(context, -2)
        );
    }

    // char literal must close here
    character = scanner_next(context);
    if (character != '\'') {
        lexer_error(context, character, '\'');
        return token_create(
            context->line,
            context->column,
            context->name,
            T_ILLEGAL,
            scanner_get_string(context, -2)
        );
    }

    // if we made it this far, the char literal must be valid!
    return token_create(
        context->line,
        context->column,
        context->name,
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
            lexer_error(context, character, '\"');
            return token_create(
                context->line,
                context->column,
                context->name,
                T_ILLEGAL,
                scanner_get_string(context, -1 - length)
            );
        }

    }

    // we made it this far; must be OK
    return token_create(
        context->line,
        context->column,
        context->name,
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

    lexer_error(context, escape_modifier, -1);
    return -1;
}

/**
 * Checks to see if the input string is a reserved keyword.
 */
bool lexer_identifier_is_keyword(char* identifier)
{
    // loop over all keywords for a match
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (strcmp(identifier, keyword_identifiers[i]) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * Creates a token based on the passed in keyword.
 */
Token lexer_create_keyword_token(char* keyword, ScannerContext* context)
{
    // loop over all keywords for a match
    for (int i = 0; i < KEYWORD_COUNT; i++) {
        if (strcmp(keyword, keyword_identifiers[i]) == 0) {
            return token_create(
                context->line,
                context->column,
                context->name,
                keyword_token_types[i],
                keyword
            );
        }
    }
    return token_create(context->line, context->column, context->name, T_ILLEGAL, keyword);
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
 * Turns a character into a printable string.
 */
char* lexer_char_printable(char character, bool quoted)
{
    char* string = malloc(sizeof(char) * 6);

    if (quoted && character == '\\') {
        strcpy(string, "'\\\\'");
    } else if (quoted && character == '\t') {
        strcpy(string, "'\\t'");
    } else if (quoted && character == '\n') {
        strcpy(string, "'\\n'");
    } else if (isprint(character)) {
        sprintf(string, "'%c'", character);
    } else {
        sprintf(string, "0x%X", character);
    }

    return string;
}

/**
 * Displays an error message for an invalid character.
 */
void lexer_error(ScannerContext* context, char unexpected, char expected)
{
    printf("%s line %d:%d: ", basename(context->name), context->line, context->column);
    if (expected >= 0) {
        printf("expecting %s, found %s\n", lexer_char_printable(expected, true), lexer_char_printable(unexpected, true));
    } else {
        printf("unexpected char: %s\n", lexer_char_printable(unexpected, false));
    }
}
