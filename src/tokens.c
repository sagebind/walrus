#include <stdlib.h>
#include <string.h>
#include "tokens.h"

/**
 * Creates a new token.
 *
 * @param  line   The line number of the token.
 * @param  column The column number of the token.
 * @param  type   The token type.
 * @param  lexeme The token lexeme.
 * @return        A newly created token.
 */
Token token_create(int line, int column, TokenType type, char* lexeme)
{
    // create the token
    Token token = {line, column, type, NULL};

    // allocate and copy the lexeme string
    token.lexeme = (char*)malloc(sizeof(char) * strlen(lexeme));
    strcpy(token.lexeme, lexeme);

    return token;
}

/**
 * Creates a new token stream of a given size.
 *
 * @param  size The size of the token stream, or the max number of tokens it can hold.
 * @return      A shiny new token stream.
 */
TokenStream* token_stream_create(int size)
{
    // allocate the stream object
    TokenStream* stream = (TokenStream*)malloc(sizeof(TokenStream));
    stream->size = size;
    stream->length = 0;

    // allocate tokens for the given size
    stream->tokens = (Token*)malloc(sizeof(Token) * size);

    return stream;
}

/**
 * Destroys a token stream and all its tokens and frees its memory.
 *
 * @param stream The token stream to destroy.
 */
void token_stream_destroy(TokenStream* stream)
{
    // free memory for tokens
    free(stream->tokens);

    // free memory for stream
    free(stream);
}
