#ifndef WALRUS_TOKENS_H
#define WALRUS_TOKENS_H

#include "error.h"


/**
 * Types of lexical tokens.
 */
typedef enum {
    T_BOOLEAN,
    T_BOOLEAN_LITERAL,
    T_BRACE_LEFT,
    T_BRACE_RIGHT,
    T_BRACKET_LEFT,
    T_BRACKET_RIGHT,
    T_BREAK,
    T_CALLOUT,
    T_CHAR_LITERAL,
    T_CLASS,
    T_COMMA,
    T_CONTINUE,
    T_ELSE,
    T_EOF,
    T_FOR,
    T_IDENTIFIER,
    T_IF,
    T_ILLEGAL,
    T_INT,
    T_INT_LITERAL,
    T_OPERATOR,
    T_PAREN_LEFT,
    T_PAREN_RIGHT,
    T_PROGRAM,
    T_RESERVED,
    T_RETURN,
    T_STATEMENT_END,
    T_STRING_LITERAL,
    T_VOID,
    T_WHITESPACE
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
 * A node in the token stream linked list.
 */
typedef struct TokenStreamNode {
    /**
     * Pointer to the previous node in the stream.
     */
    struct TokenStreamNode* previous;

    /**
     * Pointer to the next node in the stream.
     */
    struct TokenStreamNode* next;

    /**
     * The token at the current node.
     */
    Token token;
} TokenStreamNode;

/**
 * A structure storing a stream of tokens.
 */
typedef struct {
    /**
     * A pointer to the first node in the stream.
     */
    TokenStreamNode* head;

    /**
     * A pointer to the last node in the stream.
     */
    TokenStreamNode* tail;

    /**
     * The number of tokens in the token stream.
     */
    int length;
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
 * Creates a new token stream.
 *
 * @return A pointer to a shiny new token stream.
 */
TokenStream* token_stream_create(void);

/**
 * Pushes a token onto the end of a token stream.
 *
 * @param  stream The stream to push onto.
 * @param  token  The token to push.
 * @return        An error code.
 */
Error token_stream_push(TokenStream* stream, Token token);

/**
 * Destroys a token stream and all its tokens and frees its memory.
 *
 * @param  stream The token stream to destroy.
 * @return        An error code.
 */
Error token_stream_destroy(TokenStream** stream);

#endif
