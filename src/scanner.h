#ifndef SCANNER_H
#define SCANNER_H

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
    T_FALSE,
    T_FOR,
    T_IDENTIFIER,
    T_IF,
    T_INT,
    T_INT_LITERAL,
    T_RETURN,
    T_STRING_LITERAL,
    T_TRUE,
    T_VOID
} TokenType;

#endif
