#ifndef WALRUS_UTILITY_H
#define WALRUS_UTILITY_H

#include <stdbool.h>
#include "tokens.h"
#include "scanner.h"
/**
 * Checks to see if the input string is a reserved keyword.
 *
 * @param inputString The string to check.
 */
bool is_reserved(char* inString);

/**
 * Creates a token based on the passed in keyword
 *
 * @param keyword The input keyword
 * @param context The current context of the scanner
 */
Token create_keyword_token(char* in_keyword, ScannerContext* context);

#endif
