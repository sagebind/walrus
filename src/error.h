#ifndef WALRUS_ERROR_H
#define WALRUS_ERROR_H


/**
 * A list of error types.
 */
typedef enum {
    E_SUCCESS,
    E_UNKNOWN,
    E_BAD_POINTER,
    E_NO_INPUT_FILES,
    E_FILE_NOT_FOUND,
    E_LEXER_ERROR
} Error;


/**
 * Gets the error code of the most recent error.
 *
 * @return The error code of the most recent error.
 */
Error error_get_last(void);

/**
 * Sets the global error variable and displays an error message.
 *
 * @param  code The error code to exit with.
 * @param  code The error message.
 * @return      The error code given.
 */
Error error(Error code, char* message);

/**
 * Exits the program with an error message.
 *
 * @param code The error code to exit with.
 * @param code The error message.
 */
void error_exit(Error code, char* message);

#endif
