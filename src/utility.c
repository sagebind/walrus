#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

/**
 * Checks to see if the input string is a reserved keyword.
 */
bool is_reserved(char* inString)
{
    return((strcmp(inString, "boolean")==0)||(strcmp(inString, "int")==0)||(strcmp(inString, "break")==0)||(strcmp(inString, "continue")==0)||(strcmp(inString, "callout")==0)||(strcmp(inString, "class")==0)||(strcmp(inString, "if")==0)||
            (strcmp(inString, "else")==0)||(strcmp(inString, "true")==0)||(strcmp(inString, "false")==0)||(strcmp(inString, "for")==0)||(strcmp(inString, "return")==0)||(strcmp(inString, "void")==0));
}

/**
 * Creates a token based on the passed in keyword
 */
Token create_keyword_token(char* in_keyword, ScannerContext* context)
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
