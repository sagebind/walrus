#include <stdio.h>
#include <stdlib.h>
#include "error.h"

/**
 * Exits the program with an error message.
 */
void error_exit(Error code)
{
    fprintf(stderr, "Exited with error code %d\n", code);
    exit(code);
}
