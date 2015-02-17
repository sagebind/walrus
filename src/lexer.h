#ifndef WALRUS_LEXER_H
#define WALRUS_LEXER_H

#include "tokens.h"

/**
 * Tokenizes source code read from a scanner context.
 *
 * @param  context The scanner context to read from.
 * @return         A stream of tokens that represent the given source code.
 */
TokenStream* lexer_tokenize(ScannerState* context);

/**
 * Parses the next token from a scanner context.
 *
 * @param  context The scanner context to read from.
 * @return         A token parsed from the scanner input.
 */
Token lexer_next(ScannerState*);

/**
 * Prints some tokens to standard output.
 *
 * @param tokens A stream of tokens to print.
 */
void lexer_print_tokens(TokenStream*);

#endif
