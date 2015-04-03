#ifndef WALRUS_TOKENS_H
#define WALRUS_TOKENS_H

#include "error.h"


/**
 * Types of lexical tokens.
 */
typedef enum {
    T_BOOLEAN,              // boolean
    T_BOOLEAN_LITERAL,      // true, false
    T_BRACE_LEFT,           // {
    T_BRACE_RIGHT,          // }
    T_BRACKET_LEFT,         // [
    T_BRACKET_RIGHT,        // ]
    T_BREAK,                // break
    T_CALLOUT,              // callout
    T_CHAR_LITERAL,         // 'a'
    T_CLASS,                // class
    T_COMMA,                // ,
    T_CONTINUE,             // continue
    T_DIVIDE,               // /
    T_ELSE,                 // else
    T_EOF,
    T_EQUAL,                // =
    T_FOR,                  // for
    T_IDENTIFIER,           // foo, bar, baz
    T_IF,                   // if
    T_ILLEGAL,              // any invalid string of chars
    T_INT,                  // int
    T_INT_LITERAL,          // 42, 0xB055
    T_IS_EQUAL,             // ==
    T_IS_GREATER,           // >
    T_IS_GREATER_OR_EQUAL,  // >=
    T_IS_LESSER,            // <
    T_IS_LESSER_OR_EQUAL,   // <=
    T_IS_NOT_EQUAL,         // !=
    T_LOGICAL_AND,          // &&
    T_LOGICAL_NOT,          // !
    T_LOGICAL_OR,           // ||
    T_MINUS,                // -
    T_MINUS_EQUAL,          // -=
    T_MODULO,               // %
    T_MULTIPLY,             // *
    T_PAREN_LEFT,           // (
    T_PAREN_RIGHT,          // )
    T_PLUS,                 // +
    T_PLUS_EQUAL,           // +=
    T_RETURN,               // return
    T_STATEMENT_END,        // ;
    T_STRING_LITERAL,       // "hello"
    T_VOID,                 // void
    T_WHITESPACE            // any string of whitespace characters
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
     * The source file the token came from.
     */
    char* file;

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
 * @param  file   The source file the token came from.
 * @param  type   The token type.
 * @param  lexeme The token lexeme.
 * @return        A newly created token.
 */
Token token_create(int line, int column, char* file, TokenType type, char* lexeme);

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
