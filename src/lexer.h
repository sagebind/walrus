#ifndef WALRUS_LEXER_H
#define WALRUS_LEXER_H

#include "tokens.h"
#include "scanner.h"


/**
 * A position-sensitive lexer context that can be backtracked.
 */
typedef struct {
    /**
     * The scanner context to read from.
     */
    ScannerContext* context;

    /**
     * A token stream where tokens are pushed onto for backtracking.
     */
    TokenStream* tokens;

    /**
     * A pointer to the current position in the token stream.
     */
    TokenStreamNode* current_node;
} Lexer;

/**
 * Creates a new stateful lexer instance.
 *
 * @param  context The scanner that the lexer should read from.
 * @return         A new stateful lexer that can read tokens.
 */
Lexer* lexer_create(ScannerContext* context);

/**
 * Gets the next token from a lexer and advances forward one token.
 *
 * @param  lexer The lexer context.
 * @return       A token parsed from the scanner input.
 */
Token lexer_next(Lexer* lexer);

/**
 * Backtracks a lexer by one token.
 *
 * @param  lexer The lexer to backtrack.
 * @return       An error code.
 */
Error lexer_backtrack(Lexer* lexer);

/**
 * Looks ahead a given number of tokens.
 *
 * @param  lexer The lexer to look ahead in.
 * @param  count The number of tokens ahead to look.
 * @return       A token parsed from the scanner input.
 */
Token lexer_lookahead(Lexer* lexer, int count);

/**
 * Destroys an open lexer.
 *
 * Note: this function does not close the scanner context associated with the
 * lexer. You must do that separately with `scanner_close()`.
 *
 * @param  lexer The lexer to destroy.
 * @return       An error code.
 */
Error lexer_destroy(Lexer** lexer);

/**
 * Reads the next token from a scanner context.
 *
 * @param  context The scanner context to read from.
 * @return         A token parsed from the scanner input.
 */
Token lexer_read_token(ScannerContext* context);

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
