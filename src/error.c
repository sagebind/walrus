#include <stdio.h>
#include <stdlib.h>
#include "error.h"


/**
 * Exits the program with an error message.
 */
void error_exit(Error code, char* message)
{
    fprintf(stderr, "Error(%d): %s\n", code, message);
    exit(code);
}
