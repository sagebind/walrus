#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

/**
 * Checks to see if the input string is a reserved keyword.
 */
bool is_reserved(char* inString)
{
    return(inString=="boolean"||inString=="int"||inString=="break"||inString=="continue"||inString=="callout"||inString=="class"||inString=="if"||
            inString=="else"||inString=="true"||inString=="false"||inString=="for"||inString=="return"||inString=="void");
}

/**
 * Creates a token based on the passed in keyword
 */
Token create_keyword_token(char** in_keyword, ScannerContext* context)
{
	Token token;
	//check in_keyword against list of all keywords here; if a match is found, return the appropriate token	


	//in_keyword didn't match any of the known keywords - return illegal token
	token = token_create(
			context->line,
			context->column,
			T_ILLEGAL,
			in_keyword
		);
	return token;
}
