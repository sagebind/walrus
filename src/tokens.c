#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "tokens.h"

/**
 * Creates a new token.
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
 * Creates a new token stream.
 */
TokenStream* token_stream_create()
{
    // allocate the stream object
    TokenStream* stream = (TokenStream*)malloc(sizeof(TokenStream));
    stream->length = 0;
    return stream;
}

/**
 * Pushes a token onto the end of a token stream.
 */
Error token_stream_push(TokenStream* stream, Token token)
{
    // make sure pointer isn't null
    if (stream == NULL) {
        return E_BAD_POINTER;
    }

    if (stream->head == NULL) {
        stream->head = (TokenStreamNode*)malloc(sizeof(TokenStreamNode));
        stream->head->token = token;
    } else {
        TokenStreamNode* current = stream->head;

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = (TokenStreamNode*)malloc(sizeof(TokenStreamNode));
        current->next->token = token;
    }

    return E_SUCCESS;
}

/**
 * Destroys a token stream and all its tokens and frees its memory.
 */
Error token_stream_destroy(TokenStream* stream)
{
    // make sure pointer isn't null
    if (stream == NULL) {
        return E_BAD_POINTER;
    }

    // make pointers to the nodes we are currently at
    TokenStreamNode* current = stream->head;
    TokenStreamNode* next;

    // run until the last node
    while (current != NULL) {
        // advance to the next node
        next = current->next;
        // destroy current node
        free(current);
        // get the next node before we destroy the current
        current = next;
    }

    // free memory for stream
    free(stream);

    return E_SUCCESS;
}
