#ifndef WALRUS_LEXER_H
#define WALRUS_LEXER_H

#include "tokens.h"
#include "scanner.h"


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
 * Reads an integer literal token in the current context.
 *
 * @param  context The scanner context to read from.
 * @return         A matched integer token, or an illegal token.
 */
Token lexer_lex_int(ScannerContext* context);

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
 * Checks to see if an identifier string is a reserved keyword.
 *
 * @param  identifier The identifier string to check.
 * @return            True if the given identifier is a keyword, otherwise false.
 */
bool lexer_identifier_is_keyword(char* identifier);

/**
 * Creates a token based on the passed in keyword.
 *
 * @param  keyword The input keyword.
 * @param  context The current context of the scanner.
 * @return         A token for the given keyword.
 */
Token lexer_create_keyword_token(char* keyword, ScannerContext* context);

/**
 * Prints a token to standard output.
 *
 * @param tokens A token to print.
 */
void lexer_print_token(Token);

/**
 * Displays an error message for lexing errors.
 *
 * @param message The error message.
 * @param context The scanner context the error occurred at.
 */
void lexer_error(ScannerContext* context, char unexpected, char expected);

#endif
