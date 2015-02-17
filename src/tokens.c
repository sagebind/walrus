#include <stdlib.h>
#include <string.h>
#include "error.h"
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
    token.lexeme = (char*)malloc(strlen(lexeme) + 1);
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
Error token_stream_destroy(TokenStream* stream)
{
    // make sure pointer isn't null
    if (stream == NULL) {
        return E_BAD_POINTER;
    }

    // free memory for tokens
    free(stream->tokens);

    // free memory for stream
    free(stream);

    return E_SUCCESS;
}

/**
 * Pushes a token onto the end of a token stream.
 *
 * @param stream The stream to push onto.
 * @param token  The token to push.
 */
Error token_stream_push(TokenStream* stream, Token token)
{
    // make sure pointer isn't null
    if (stream == NULL) {
        return E_BAD_POINTER;
    }

    // not enough room, reallocate to make more room
    if (stream->length >= stream->size) {
        stream->size *= 2; // increase size 2x
        stream->tokens = (Token*)realloc(stream->tokens, sizeof(Token) * stream->size);
    }

    stream->tokens[stream->length++] = token;
    return E_SUCCESS;
}
