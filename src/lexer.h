#ifndef WALRUS_LEXER_H
#define WALRUS_LEXER_H

#include "tokens.h"
#include "scanner.h"

/**
 * Tokenizes source code read from a scanner context.
 *
 * @param  context The scanner context to read from.
 * @return         A stream of tokens that represent the given source code.
 */
TokenStream* lexer_tokenize(ScannerContext* context);

/**
 * Parses the next token from a scanner context.
 *
 * @param  context The scanner context to read from.
 * @return         A token parsed from the scanner input.
 */
Token lexer_next(ScannerContext* context);

/**
 * Reads an identifier token in the current context.
 *
 * @param  context The scanner context to read from.
 * @return         A matched char token, or an illegal token.
 */
Token lexer_lex_identifier(ScannerContext* context);

/**
 * Reads a char token in the current context.
 *
 * @param  context The scanner context to read from.
 * @return         A matched char token, or an illegal token.
 */
Token lexer_lex_char(ScannerContext* context);

/**
 * Reads a string token in the current context.
 *
 * @param  context The scanner context to read from.
 * @return         A matched string token, or an illegal token.
 */
Token lexer_lex_string(ScannerContext* context);

/**
 * Scans an escaped character and returns the actual character, or -1 if the char is invalid.
 *
 * @param  context The scanner context to read from.
 * @return         The escaped character.
 */
char lexer_scan_escaped(ScannerContext* context);

/**
 * Prints some tokens to standard output.
 *
 * @param tokens A stream of tokens to print.
 */
void lexer_print_tokens(TokenStream*);

/**
 * Displays an error message for lexing errors.
 *
 * @param message The error message.
 * @param context The scanner context the error occurred at.
 */
void lexer_error(char* message, ScannerContext* context);

#endif
