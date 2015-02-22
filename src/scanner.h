#ifndef WALRUS_SCANNER_H
#define WALRUS_SCANNER_H

#include <stdio.h>
#include "error.h"

/**
 * A context structure for maintaining scanner state.
 */
typedef struct {
    /**
     * An open file stream.
     */
    FILE* stream;

    /**
     * The current line in the source file.
     */
    int line;

    /**
     * The current text column in the source file.
     */
    int column;

    /**
     * Indicates if we are approaching the end of a line.
     */
    bool eol;
} ScannerContext;

/**
 * Opens a file to be scanned and returns a scanner context for the file.
 *
 * @param  filename The name of the file to open.
 * @return          A new scanner context.
 */
ScannerContext* scanner_open(char* filename);

/**
 * Creates a scanner context for reading from a string.
 *
 * @param  string The string to scan from.
 * @return        A new scanner context.
 */
ScannerContext* scanner_open_string(char* string);

/**
 * Reads a next character from a scanner context and advances to the next character.
 *
 * @param  context An open scanner context.
 * @return         The next character in the stream.
 */
char scanner_next(ScannerContext* context);

/**
 * Reads a character from a scanner context relative to the current position.
 *
 * @param  context An open scanner context.
 * @param  offset  The character offset from the current position.
 * @return         The next character in the stream.
 */
char scanner_peek(ScannerContext* context, long int offset);

/**
 * Moves the current position from the current one by an offset.
 *
 * @param  context An open scanner context.
 * @param  offset  The character offset from the current position.
 * @return         An error code.
 */
Error scanner_backtrack(ScannerContext* context, long int offset);

/**
 * Gets the string of characters between a given offset and the current position.
 *
 * @param  context An open scanner context.
 * @param  offset  The character offset from the current position.
 * @return         A string of characters.
 */
char* scanner_get_string(ScannerContext* context, long int offset);

/**
 * Closes a scanner context.
 *
 * @param  context An open scanner context to close.
 * @return         An error code.
 */
Error scanner_close(ScannerContext* context);

#endif
