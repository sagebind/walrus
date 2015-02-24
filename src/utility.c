#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

/**
 * Checks to see if the input string is a reserved keyword.
 */
bool isReserved(char* inString)
{
    return(inString=="boolean"||inString=="int"||inString=="break"||inString=="continue"||inString=="callout"||inString=="class"||inString=="if"||
            inString=="else"||inString=="true"||inString=="false"||inString=="for"||inString=="return"||inString=="void");
}
