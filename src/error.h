#ifndef WALRUS_ERROR_H
#define WALRUS_ERROR_H


/**
 * A list of error types.
 */
typedef enum {
    E_SUCCESS = 0,
    E_UNKNOWN,
    E_BAD_POINTER,
    E_OPERATION_FAILED,
    E_UNKNOWN_OPTION,
    E_NO_INPUT_FILES,
    E_FILE_NOT_FOUND,
    E_LEXER_ERROR,
    E_PARSE_ERROR,
    E_ANALYZE_ERROR
} Error;


/**
 * Gets the error code of the most recent error.
 *
 * @return The error code of the most recent error.
 */
Error error_get_last(void);

/**
 * Gets the total number of errors that have occurred.
 *
 * @return The total number of errors that have occurred.
 */
int error_get_count(void);

/**
 * Sets the global error variable and displays an error message.
 *
 * @param  code    The error code to exit with.
 * @param  message The error message.
 * @param  ...     Message formatting arguments.
 * @return         The error code given.
 */
Error error(Error code, const char* message, ...);

/**
 * Exits the program with an error message.
 *
 * @param code The error code to exit with.
 * @param code The error message.
 */
void error_exit(Error code, const char* message);

/**
 * Clears the last error, resetting last error to 0 and decrementing error count.
 *
 * Effectively allows you to mark the last error as "caught".
 */
void error_catch(void);

#endif
