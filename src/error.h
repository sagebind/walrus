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
 * @param code The error code to exit with.
 */
void error_exit(Error code);

#endif
