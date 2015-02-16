#include <stdio.h>
#include "scanner.h"
#include "tokens.h"

/**
 * Prints some tokens to standard output.
 *
 * @param tokens A stream of tokens to print.
 */
void scanner_print_tokens(TokenStream* tokens)
{
    // loop over each token in the stream
    for (int i = 0; i < tokens->length; ++i) {
        printf("%.2d ", tokens->tokens[i].line);

        if (tokens->tokens[i].type == T_BOOLEAN_LITERAL) {
            printf("BOOLEANLITERAL ");
        } else if (tokens->tokens[i].type == T_CHAR_LITERAL) {
            printf("CHARLITERAL ");
        } else if (tokens->tokens[i].type == T_INT_LITERAL) {
            printf("INTLITERAL ");
        } else if (tokens->tokens[i].type == T_STRING_LITERAL) {
            printf("STRINGLITERAL ");
        }

        printf("%s\r\n", tokens->tokens[i].lexeme);
    }
}
