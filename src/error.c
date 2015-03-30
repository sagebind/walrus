#include <stdio.h>
#include <stdlib.h>
#include "error.h"


/**
 * Stores the error code of the most recent error.
 */
Error error_last = E_SUCCESS;

/**
 * Gets the error code of the most recent error.
 */
Error error_get_last()
{
    return error_last;
}

/**
 * Sets the global error variable and displays an error message.
 */
Error error(Error code, char* message)
{
    error_last = code;
    fprintf(stderr, "Error(%d): %s\n", code, message);
    return code;
}

/**
 * Exits the program with an error message.
 */
void error_exit(Error code, char* message)
{
    error(code, message);
    exit(code);
}
