#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"


/**
 * Stores the error code of the most recent error.
 */
static Error error_last = E_SUCCESS;

/**
 * Total number of errors that have occurred.
 */
static int error_count = 0;

/**
 * Gets the error code of the most recent error.
 */
Error error_get_last()
{
    return error_last;
}

/**
 * Gets the total number of errors that have occurred.
 */
int error_get_count()
{
    return error_count;
}

/**
 * Sets the global error variable and displays an error message.
 */
Error error(Error code, const char* message, ...)
{
    // print message to stderr and accept formatting arguments like printf does
    va_list args;
    va_start(args, message);
    fprintf(stderr, "Error(%#06x): ", code);
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);

    // update last error and error count
    error_last = code;
    error_count++;
    return code;
}

/**
 * Exits the program with an error message.
 */
void error_exit(Error code, const char* message)
{
    error(code, message);
    exit(code);
}

/**
 * Clears the last error, resetting last error to 0 and decrementing error count.
 *
 * Effectively allows you to mark the last error as "caught".
 */
void error_catch()
{
    if (error_count > 0) {
        error_last = 0;
        error_count--;
    }
}
