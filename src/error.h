#ifndef WALRUS_ERROR_H
#define WALRUS_ERROR_H

/**
 * A list of error types.
 */
typedef enum {
    E_SUCCESS,
    E_UNKNOWN,
    E_BAD_POINTER,
    E_LEXER_ERROR
} Error;

/**
 * Exits the program with an error message.
 *
 * @param code   The error code.
 * @param error  The error message to display.
 * @param line   The line number the error occurred at.
 * @param column The column number the error occurred at.
 */
void throw_error(Error code, char* error, int line, int column);


#endif
