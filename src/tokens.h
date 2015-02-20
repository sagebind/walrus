#ifndef WALRUS_TOKENS_H
#define WALRUS_TOKENS_H

#include "error.h"

/**
 * Types of lexical tokens.
 */
typedef enum {
    T_BOOLEAN,
    T_BOOLEAN_LITERAL,
    T_BREAK,
    T_CALLOUT,
    T_CHAR_LITERAL,
    T_CLASS,
    T_CONTINUE,
    T_ELSE,
    T_EOF,
    T_FALSE,
    T_FOR,
    T_IDENTIFIER,
    T_IF,
    T_ILLEGAL,
    T_INT,
    T_INT_LITERAL,
    T_OPERATOR,
    T_RETURN,
    T_STRING_LITERAL,
    T_TRUE,
    T_VOID
} TokenType;

/**
 * A structure representing a lexical token.
 */
typedef struct {
    /**
     * The line number the token was found in the source file.
     */
    int line;

    /**
     * The column number the token was found in the source file.
     */
    int column;

    /**
     * The token type of the token.
     */
    TokenType type;

    /**
     * The token lexeme; the actual text of the token.
     */
    char* lexeme;
} Token;

/**
 * A structure storing a stream of tokens.
 */
typedef struct {
    /**
     * A pointer to the start of an array of token objects.
     */
    Token* tokens;

    /**
     * The number of tokens in the token stream.
     */
    int length;

    /**
     * The maximum size of the token stream.
     */
    int size;
} TokenStream;

/**
 * Creates a new token.
 *
 * @param  line   The line number of the token.
 * @param  column The column number of the token.
 * @param  type   The token type.
 * @param  lexeme The token lexeme.
 * @return        A newly created token.
 */
Token token_create(int line, int column, TokenType type, char* lexeme);

/**
 * Creates a new token stream of a given size.
 *
 * @param  size The size of the token stream, or the max number of tokens it can hold.
 * @return      A shiny new token stream.
 */
TokenStream* token_stream_create(int size);

/**
 * Destroys a token stream and all its tokens and frees its memory.
 *
 * @param  stream The token stream to destroy.
 * @return        An error code.
 */
Error token_stream_destroy(TokenStream* stream);

/**
 * Pushes a token onto the end of a token stream.
 *
 * @param  stream The stream to push onto.
 * @param  token  The token to push.
 * @return        An error code.
 */
Error token_stream_push(TokenStream* stream, Token token);

#endif
